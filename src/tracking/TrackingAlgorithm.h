#ifndef PROY_GRADO_TRACKING_ALGORITHM_H
#define PROY_GRADO_TRACKING_ALGORITHM_H

#include <opencv2/core/utility.hpp>
#include <opencv2/tracking.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/objdetect/objdetect.hpp>
#include <opencv2/highgui.hpp>
#include <iostream>
#include <cstring>
#include <ctime>

class TrackingAlgorithm {
public:
    virtual void setTargets(std::vector<cv::Rect2d> targets, std::shared_ptr<cv::Mat> frame) = 0;
    virtual std::vector<cv::Rect2d> getTargets() = 0;
    virtual bool track(std::shared_ptr<cv::Mat> track) = 0;
};


#endif //PROY_GRADO_TRACKING_ALGORITHM_H
