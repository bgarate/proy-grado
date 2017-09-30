#include <boost/shared_ptr.hpp>
#include "C4/Pedestrian.h"
#include "DetectionAlgorithm.h"

#ifndef PROY_GRADO_C4DETECTOR_H
#define PROY_GRADO_C4DETECTOR_H


class C4Detector : public DetectionAlgorithm{

public:
    C4Detector();
    std::vector<cv::Rect2d> getFound() override;
    void detect(std::shared_ptr<cv::Mat> frame) override;

private:
    std::vector<cv::Rect2d> foundTargets;

    const int HUMAN_height = 108;
    const int HUMAN_width = 36;
    const int HUMAN_xdiv = 9;
    const int HUMAN_ydiv = 4;

    const int POST_PROCCESS = 10;
    const float STEP_SIZE = 2;

    DetectionScanner * scanner;
};

static const int EXT = 1;

#endif //PROY_GRADO_C4DETECTOR_H
