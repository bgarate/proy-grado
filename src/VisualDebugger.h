
//
// Created by bruno on 27/05/17.
//

#ifndef PROY_GRADO_VISUALDEBUGGER_H
#define PROY_GRADO_VISUALDEBUGGER_H


#include <opencv2/core/mat.hpp>
#include <memory>
#include "Config.h"
#include "Point.h"
#include <opencv/cv.hpp>
#include <src/hal/hal.hpp>

class VisualDebugger {
public:
    void setup(Config* config);
    void setFrame(std::shared_ptr<cv::Mat> frame);
    void setStatus(State state, int battery, double altitude, Point gps, Point orientation, int fps,
                   long runningTime);
    int show(long deltaTime);
    void setTracks(std::vector<Track> tracks);
    void writeConsole(std::string str);
    void cleanup();
private:
    Config* config;
    cv::Mat frame;
    std::shared_ptr<cv::Mat> originalFrame;

    static const cv::Scalar WHITE_COLOR;
    static const cv::Scalar GREEN_COLOR;
    static const cv::Scalar GREY_COLOR;

    static const int CONSOLE_QUEUE_SIZE = 5;
    static const int CONSOLE_FONT = cv::FONT_HERSHEY_PLAIN;
    static const int CONSOLE_FONT_SIZE = 1;
    static const int CONSOLE_FONT_THICKNESS = 1;
    static constexpr const double OUTPUT_FPS = 25;

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
    cv::VideoWriter rawOutput;
    cv::VideoWriter hudOutput;

    void openWriters(cv::Size frameSize);
    bool shouldOpen;
};


#endif //PROY_GRADO_VISUALDEBUGGER_H
