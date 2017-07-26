//
// Created by bruno on 23/07/17.
//

#ifndef PROY_GRADO_OPTICALFLOW_H
#define PROY_GRADO_OPTICALFLOW_H


#include <opencv2/core/mat.hpp>
#include <opencv2/video/background_segm.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <memory>

typedef struct {
    std::vector<cv::Point2f> Start;
    std::vector<cv::Point2f> End;
    double maxDistance;
    double minDistance;
    double thresholdDistance;
    cv::Mat bgMask;
    std::vector<std::vector<cv::Point2f>> Clusters;
    std::vector<cv::Rect2f> BoundingBoxes;
    std::vector<cv::Point> ClustersCenters;
    std::vector<float> Proximity;
} OpticalFlowPoints;

class OpticalFlow {
public:
    OpticalFlow();
    void Update(std::shared_ptr<cv::Mat> frame, double deltaTime);
    OpticalFlowPoints *GetPoints();

private:
    cv::Mat currentFrame;
    cv::Mat previousFrame;
    cv::Ptr<cv::BackgroundSubtractor> backgroundSubstractor;

    cv::TermCriteria termcrit;

    bool needsRetrack = true;
    double timeElapsedFromLastRetrack = 0;

    OpticalFlowPoints* Points = new OpticalFlowPoints();

    const int MAX_FEATURES = 500;
    static const cv::Size SUB_PIX_WIN_SIZE;
    static const cv::Size WIN_SIZE;

    void calculateFeatureMask() const;

    std::vector<cv::Point2f> clusterFeatures();

    static constexpr float CLUSTER_DISTANCE = 250;
    const int MINIMUM_CLUSTER_ELEMENTS = 1;

    void distanceEstimation();

    const float TEMPLATE_SCALE_STEP = 0.1;
    const int TEMPLATE_SCALE_NUMBER_STEPS = 9;
    int TEMPLATE_SIZE = 32;

    int TEMPLATE_MATCH_METHOD = cv::TM_CCOEFF;
};


#endif //PROY_GRADO_OPTICALFLOW_H
