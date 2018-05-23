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
    void detect(std::shared_ptr<cv::Mat> frame) override;

private:
    std::vector<cv::Rect2d> foundTargets;
    cv::HOGDescriptor hog;
    const float SCALE_FACTOR = 1;
};


#endif //PROY_GRADO_HOGDETECTOR_H
