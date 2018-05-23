#ifndef PROY_GRADO_DETECTION_ALGORITHM_H
#define PROY_GRADO_DETECTION_ALGORITHM_H

#include <opencv2/core/utility.hpp>
#include <opencv2/tracking.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/objdetect/objdetect.hpp>
#include <opencv2/highgui.hpp>
#include <iostream>
#include <cstring>
#include <ctime>
#include "memory"

class DetectionAlgorithm {
public:
    virtual std::vector<cv::Rect2d> getFound() = 0;
    virtual void detect(std::shared_ptr<cv::Mat> frame) = 0;
};


#endif //PROY_GRADO_DETECTION_ALGORITHM_H
