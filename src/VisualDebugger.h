
//
// Created by bruno on 27/05/17.
//

#ifndef PROY_GRADO_VISUALDEBUGGER_H
#define PROY_GRADO_VISUALDEBUGGER_H


#include <opencv2/core/mat.hpp>
#include <memory>
#include "Config.h"
#include "hal/Point.h"
#include "tracking/Track.h"
#include <opencv/cv.hpp>
#include "hal/hal.hpp"
#include "tracking/Follower.h"
#include "../lib/ORB_SLAM2/include/System.h"

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

    void setFollowCommand(FollowCommand command);
    void drawMouse(double deltaTime);

    void captureImage();

    void setOrbSlam(ORB_SLAM2::System *slam);
    void drawOrbSlam();

    void drawHorizon(int y);

private:
    Config* config;
    cv::Mat frame;
    std::shared_ptr<cv::Mat> originalFrame;

    static const cv::Scalar WHITE_COLOR;
    static const cv::Scalar GREEN_COLOR;
    static const cv::Scalar GREY_COLOR;
    static const cv::Scalar BLACK_COLOR;
    static const cv::Scalar RED_COLOR;

    static const int CONSOLE_QUEUE_SIZE = 5;
    static const int CONSOLE_FONT = cv::FONT_HERSHEY_PLAIN;
    static const int CONSOLE_FONT_SIZE = 1;
    static const int CONSOLE_FONT_THICKNESS = 1;
    static constexpr const double OUTPUT_FPS = 25;

    double altitude = 0;

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
    std::vector<Track> tracks;

    static void onMouse(int evt, int x, int y, int flag, void *thisPtr);
    cv::Point2i mousePosition;

    Follower* follower;

    ORB_SLAM2::FrameDrawer *frameDrawer = NULL;

    std::string lastState = "";


};


#endif //PROY_GRADO_VISUALDEBUGGER_H
