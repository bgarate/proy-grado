//
// Created by bruno on 23/07/17.
//

#include <opencv2/imgproc.hpp>
#include <opencv/cv.hpp>
#include "OpticalFlow.h"

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

    if(timeElapsedFromLastRetrack > 10000) // 10ms
        needsRetrack = true;

    if(needsRetrack) {
        cv::goodFeaturesToTrack(currentFrame, Points->Start, MAX_FEATURES, 0.01, 10, Points->bgMask, 3, false, 0.04);
        if(!Points->Start.empty()) {
            Points->Start = clusterFeatures();
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

std::vector<cv::Point2f> OpticalFlow::clusterFeatures() {

    if(Points->Start.size() <= 2)
        return Points->Start;

    std::sort(Points->Start.begin(),Points->Start.end(), [](const cv::Point2f &a,const cv::Point2f &b) {
        return a.x*a.x + a.y*a.y < b.x*b.x + b.y*b.y;
    });

    std::vector<cv::Point2f> keptFeatures;
    int clusterElements = 0;

    cv::Point2f point = Points->Start[0];

    for (int i = 1; i < Points->Start.size(); ++i) {
        cv::Point2f currentPoint = Points->Start[i];
        cv::Point2f v = point - currentPoint;
        float sqrdDistance = v.x*v.x + v.y*v.y;

        if(sqrdDistance < CLUSTER_DISTANCE) {
            point = currentPoint;
            clusterElements++;
        } else {
            if(clusterElements > MINIMUM_CLUSTER_ELEMENTS) {
                keptFeatures.push_back(point);
            }
            clusterElements = 0;
        }
    }

    return Points->Start;


}
