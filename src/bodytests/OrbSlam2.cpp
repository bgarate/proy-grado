//
// Created by bruno on 18/05/17.
//

#include <zconf.h>
#include "../logging/Logger.h"
#include "../tracking/DetectAndTrack.h"
#include "../tracking/HogDetector.h"
#include "../tracking/MultiTracker.h"
#include "../VisualDebugger.h"
#include "../../lib/ORB_SLAM2/include/System.h"
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
        const std::string voc = "lib/ORB_SLAM2/Vocabulary/ORBvoc.txt";
        const std::string settings ="lib/ORB_SLAM2/Examples/Monocular/CustomCalib.yaml";
        SLAM = new ORB_SLAM2::System(voc,settings, ORB_SLAM2::System::MONOCULAR,true);
        visualDebugger->setOrbSlam(SLAM);
    }

    bool BodyTestStep(double deltaTime) override {

        std::shared_ptr<cv::Mat> frame = hal->getFrame(Camera::Front);
        if(frame != NULL && frame->rows > 0 & frame->cols > 0) {
            cv::Mat grey;
            //cv::cvtColor(*frame, grey, CV_BGR2GRAY);
            runningTime += deltaTime;
            SLAM->TrackMonocular(*frame, runningTime / (double)1000000.0);
        }

        return true;

    }

    void FinishBodyTest() override {
        if(hal->getState() != State::Landed)
            hal->land();
    }

};