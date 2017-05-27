
//
// Created by bruno on 27/05/17.
//

#ifndef PROY_GRADO_VISUALDEBUGGER_H
#define PROY_GRADO_VISUALDEBUGGER_H


#include <opencv2/core/mat.hpp>
#include <memory>
#include "Config.h"
#include <opencv/cv.hpp>
#include <src/tracking/Track.h>

class VisualDebugger {
public:
    void setup(Config* config);
    void setFrame(std::shared_ptr<cv::Mat> frame);
    int show();
    void setTracks(std::vector<Track> tracks);
private:
    Config* config;
    cv::Mat frame;

    std::array<cv::Scalar, 9> colors ={{ cv::Scalar(255,0,0),
                         cv::Scalar(0,255,0),
                         cv::Scalar(0,0,255),
                         cv::Scalar(120,120,0),
                         cv::Scalar(0,120,120),
                         cv::Scalar(120,0,120),
                         cv::Scalar(255,0,120),
                         cv::Scalar(0,120,255),
                         cv::Scalar(255,120,0)
                                               }};

    std::string windowName;

};


#endif //PROY_GRADO_VISUALDEBUGGER_H
