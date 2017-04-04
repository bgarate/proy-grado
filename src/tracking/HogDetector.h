//
// Created by bruno on 01/04/17.
//

#include "DetectionAlgorithm.h"

#ifndef PROY_GRADO_HOGDETECTOR_H
#define PROY_GRADO_HOGDETECTOR_H


class HogDetector : public DetectionAlgorithm{
public:
    HogDetector();
    std::vector<cv::Rect2d> getFound() override;
    void detect(cv::Mat frame) override;

private:
    std::vector<cv::Rect2d> foundTargets;
    cv::HOGDescriptor hog;
    const float SCALE_FACTOR = 1;
    const double OVERLAP_THRESHOLD = 0.6;
    void filter_rects(std::vector<cv::Rect> &candidates, std::vector<cv::Rect2d> &objects);
};


#endif //PROY_GRADO_HOGDETECTOR_H
