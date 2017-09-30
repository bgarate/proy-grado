#ifndef PROY_GRADO_COMBC4CASCADEDETECTOR_H
#define PROY_GRADO_COMBC4CASCADEDETECTOR_H


#include <boost/shared_ptr.hpp>
#include "DetectionAlgorithm.h"

class CombC4CascadeDetector : public DetectionAlgorithm {

public:
    CombC4CascadeDetector();
    std::vector<cv::Rect2d> getFound() override;
    void detect(std::shared_ptr<cv::Mat> frame) override;

private:
    std::vector<cv::Rect2d> foundTargets;
    DetectionAlgorithm* c4Detector;
    DetectionAlgorithm* cascadeDetector;

};


#endif //PROY_GRADO_COMBC4CASCADEDETECTOR_H
