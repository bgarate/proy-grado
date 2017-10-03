//
// Created by bruno on 23/07/17.
//

#include <opencv2/imgproc.hpp>
#include <opencv/cv.hpp>
#include "OpticalFlow.h"
#include "DbScan.h"
#include "src/utils/Helpers.h"

const cv::Size OpticalFlow::SUB_PIX_WIN_SIZE = cv::Size(10,10);
const cv::Size OpticalFlow::WIN_SIZE = cv::Size(31,31);

OpticalFlow::OpticalFlow():
        termcrit(cv::TermCriteria::COUNT|cv::TermCriteria::EPS,20,0.03) {
    backgroundSubstractor = cv::createBackgroundSubtractorMOG2(500,16,true);

}

void OpticalFlow::Update(std::shared_ptr<cv::Mat> frame, double deltaTime) {

    if(frame == NULL || frame->empty())
        return;

    if(!previousFrame.empty()) {
        std::swap(Points->End, Points->Start);
        cv::swap(previousFrame, currentFrame);
    }

    cv::cvtColor(*frame, currentFrame, cv::COLOR_BGR2GRAY);

    calculateFeatureMask();

    if(previousFrame.empty())
        currentFrame.copyTo(previousFrame);

    timeElapsedFromLastRetrack += deltaTime;

    if(timeElapsedFromLastRetrack > 100000) // 100ms
        needsRetrack = true;

    if(needsRetrack) {
        cv::goodFeaturesToTrack(currentFrame, Points->Start, MAX_FEATURES, 0.01, 10, Points->bgMask, 3, false, 0.04);
        Points->Start = clusterFeatures();
        if(!Points->Start.empty()) {
            cv::cornerSubPix(currentFrame, Points->Start, SUB_PIX_WIN_SIZE, cv::Size(-1, -1), termcrit);
            needsRetrack = false;
            timeElapsedFromLastRetrack = 0;
        }
    }

    if (!Points->Start.empty()) {

        std::vector<uchar> status;
        std::vector<float> err;

        cv::calcOpticalFlowPyrLK(previousFrame, currentFrame, Points->Start, Points->End, status, err, WIN_SIZE,
                                 3, termcrit, 0, 0.001);

        int k = 0;

        Points->maxDistance = 0;
        Points->minDistance = 99999;

        for (int i = 0; i < Points->Start.size(); ++i) {

            if(!status[i])
                continue;

            Points->Start[k] = Points->Start[i];
            Points->End[k] = Points->End[i];

            cv::Point2f v = Points->End[i] - Points->Start[i];
            double distance = std::sqrt(v.x*v.x + v.y*v.y);

            Points->maxDistance = std::max(distance, Points->maxDistance);
            Points->minDistance = std::min(distance, Points->minDistance);

            k++;

        }

        Points->Start.resize(k);
        Points->End.resize(k);

        Points->thresholdDistance = Points->minDistance + (Points->maxDistance - Points->minDistance) * 1 / 3;

        OpticalFlow::distanceEstimation();

    }
}

void OpticalFlow::calculateFeatureMask() const {
    backgroundSubstractor->apply(currentFrame, Points->bgMask, 0.1);

    std::vector<std::vector<cv::Point>> contours;

    findContours(Points->bgMask, contours, cv::RETR_LIST, cv::CHAIN_APPROX_SIMPLE);
    Points->bgMask.setTo(cv::Scalar(0, 0, 0));

    for (int j = 0; j < contours.size(); ++j) {
        if(contourArea(contours[j]) > 5){
            drawContours(Points->bgMask, contours, j, cv::Scalar(255, 255, 255), cv::FILLED, cv::LINE_8);
        }
    }
}

OpticalFlowPoints* OpticalFlow::GetPoints() {
    return Points;
}

void OpticalFlow::distanceEstimation() {

    DbScan dbScan(&Points->Start, 30, 3);

    Points->Clusters = dbScan.run();
    Points->BoundingBoxes.clear();
    Points->ClustersCenters.clear();
    Points->Proximity.clear();

    for(std::vector<cv::Point2f> cluster : Points->Clusters) {
        Points->BoundingBoxes.push_back(cv::boundingRect(cluster));
        cv::Point2f sum = std::accumulate(cluster.begin(), cluster.end(), cv::Point2f(0,0));
        cv::Point center = cv::Point((int)(sum.x/cluster.size()),(int)(sum.y/cluster.size()));
        Points->ClustersCenters.push_back(center);
    }

    for (int i = 0; i < Points->Clusters.size(); ++i) {

        cv::Point center = Points->ClustersCenters[i];
        cv::Rect templateBox(center.x - TEMPLATE_SIZE / 2, center.y - TEMPLATE_SIZE / 2,
                             TEMPLATE_SIZE, TEMPLATE_SIZE);

        templateBox = Helpers::Clip(templateBox, currentFrame.size());

        cv::Mat current = currentFrame(templateBox);

        bool minimizingMethod = TEMPLATE_MATCH_METHOD  == CV_TM_SQDIFF || TEMPLATE_MATCH_METHOD == CV_TM_SQDIFF_NORMED;

        float bestScale = 0;
        double bestScore = 0;

        for (int j = 1; j <= TEMPLATE_SCALE_NUMBER_STEPS; ++j) {
            float scale = 1 + j * TEMPLATE_SCALE_STEP;
            cv::Mat scaledPrevious;
            cv::resize(previousFrame(templateBox), scaledPrevious, cv::Size(), scale, scale);

            cv::Mat result;

            int result_cols = scaledPrevious.cols - current.cols + 1;
            int result_rows = scaledPrevious.rows - current.rows + 1;

            cv::matchTemplate(scaledPrevious, current, result, TEMPLATE_MATCH_METHOD);
            result.create(result_rows, result_cols, CV_32FC1);

            double minScore;
            double maxScore;

            cv::minMaxLoc(result, &minScore, &maxScore);

            if((minimizingMethod && -minScore > bestScore) ||
                    (!minimizingMethod && maxScore > bestScore)) {
                bestScore = minimizingMethod ? -minScore : maxScore;
                bestScale = scale;
            }

        }

        Points->Proximity.push_back(bestScale - 1);

    }

}

std::vector<cv::Point2f> OpticalFlow::clusterFeatures() {

    if(Points->Start.size() <= 2)
        return Points->Start;

    std::sort(Points->Start.begin(),Points->Start.end(), [](const cv::Point2f &a,const cv::Point2f &b) {
        return a.x*a.x + a.y*a.y < b.x*b.x + b.y*b.y;
    });

    std::vector<cv::Point2f> keptFeatures;
    int clusterElements = 0;

    cv::Point2f previousPoint = Points->Start[0];

    for (int i = 1; i < Points->Start.size(); ++i) {
        cv::Point2f currentPoint = Points->Start[i];
        cv::Point2f v = currentPoint - previousPoint;

        float sqrdDistance = v.x*v.x + v.y*v.y;

        if(sqrdDistance < CLUSTER_DISTANCE) {
            clusterElements++;
        } else {
            if(clusterElements >= MINIMUM_CLUSTER_ELEMENTS) {
                keptFeatures.push_back(currentPoint);
            }
            clusterElements = 1;
        }

        previousPoint = currentPoint;
    }

    if(clusterElements > MINIMUM_CLUSTER_ELEMENTS)
        keptFeatures.push_back(previousPoint);

    return keptFeatures;


}
