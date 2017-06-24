//
// Created by bruno on 01/04/17.
//

#include "DetectionAlgorithm.h"

#ifndef PROY_GRADO_CASCADEDETECTOR_H
#define PROY_GRADO_CASCADEDETECTOR_H


class CascadeDetector : public DetectionAlgorithm{
public:
    CascadeDetector();
    std::vector<cv::Rect2d> getFound() override;
    void detect(std::shared_ptr<cv::Mat> frame) override;

private:
    std::vector<cv::Rect2d> foundTargets;
    cv::CascadeClassifier cascade;
    const float SCALE_FACTOR = 1;
};


#endif //PROY_GRADO_CASCADEDETECTOR_H
