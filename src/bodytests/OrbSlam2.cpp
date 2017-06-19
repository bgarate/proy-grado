//
// Created by bruno on 18/05/17.
//

#include <zconf.h>
#include <src/logging/Logger.h>
#include <src/tracking/DetectAndTrack.h>
#include <src/tracking/HogDetector.h>
#include <src/tracking/MultiTracker.h>
#include <src/VisualDebugger.h>
#include <lib/ORB_SLAM2/include/System.h>
#include "BodyTest.h"
#include "../hal/hal.hpp"

class OrbSlam2 : public BodyTest {

    Hal* hal;
    int currentStep = 0;
    double currentTime = 0;

    ORB_SLAM2::System* SLAM;

    VisualDebugger* visualDebugger;

    long runningTime = 0;

    void InitBodyTest(Hal *hal, Config* config, VisualDebugger* visualDebugger) override {
        this->hal = hal;
        this->visualDebugger = visualDebugger;
        const std::string voc = "/home/santy/Escritorio/Proyecto/Git/proy-grado/lib/ORB_SLAM2/Vocabulary/ORBvoc.txt";
        const std::string settings ="/home/santy/Escritorio/Proyecto/Git/proy-grado/lib/ORB_SLAM2/Examples/Monocular/TUM1.yaml";
        SLAM = new ORB_SLAM2::System(voc,settings, ORB_SLAM2::System::MONOCULAR,true);
    }

    bool BodyTestStep(double deltaTime) override {

        std::shared_ptr<cv::Mat> frame = hal->getFrame(Camera::Front);
        if(frame != NULL && frame->rows > 0 & frame->cols > 0) {
            cv::Mat grey;
            cv::cvtColor(*frame, grey, CV_BGR2GRAY);
            runningTime += deltaTime;
            SLAM->TrackMonocular(grey, runningTime / 1000.0);
        }

        return true;

    }

    void FinishBodyTest() override {
        if(hal->getState() != State::Landed)
            hal->land();
    }

};