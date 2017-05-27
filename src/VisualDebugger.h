
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
#include <src/hal/hal.hpp>

class VisualDebugger {
public:
    void setup(Config* config);
    void setFrame(std::shared_ptr<cv::Mat> frame);
    void setStatus(State state, int battery, double altitude, Point gps, Point orientation);
    int show();
    void setTracks(std::vector<Track> tracks);
    void writeConsole(std::string str);
    void cleanup();
private:
    Config* config;
    cv::Mat frame;

    static const cv::Scalar WHITE_COLOR;
    static const int CONSOLE_QUEUE_SIZE = 5;
    static const int CONSOLE_FONT = cv::FONT_HERSHEY_PLAIN;
    static const int CONSOLE_FONT_SIZE = 1;
    static const int CONSOLE_FONT_THICKNESS = 1;

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
    std::vector<std::string> console;

    static std::string getStateName(State state);

    cv::Scalar getStateColor(State state);
};


#endif //PROY_GRADO_VISUALDEBUGGER_H
