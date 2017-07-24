//
// Created by bruno on 23/07/17.
//

#ifndef PROY_GRADO_OPTICALFLOW_H
#define PROY_GRADO_OPTICALFLOW_H


#include <opencv2/core/mat.hpp>
#include <opencv2/video/background_segm.hpp>
#include <memory>

typedef struct {
    std::vector<cv::Point2f> Start;
    std::vector<cv::Point2f> End;
    double maxDistance;
    double minDistance;
    double thresholdDistance;
    cv::Mat bgMask;
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

    static constexpr float CLUSTER_DISTANCE = 500;
    const int MINIMUM_CLUSTER_ELEMENTS = 3;
};


#endif //PROY_GRADO_OPTICALFLOW_H
