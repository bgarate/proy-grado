//
// Created by bruno on 18/05/17.
//

#include <zconf.h>
#include <src/tracking/OpticalFlow.h>
#include "../logging/Logger.h"
#include "../tracking/DetectAndTrack.h"
#include "../tracking/HogDetector.h"
#include "../tracking/MultiTracker.h"
#include "../VisualDebugger.h"
#include "../../lib/ORB_SLAM2/include/System.h"
#include "BodyTest.h"
#include "../hal/hal.hpp"

class OpticalFlowObstacleAvoidance : public BodyTest {

    Hal* hal;
    int currentStep = 0;
    double currentTime = 0;

    VisualDebugger* visualDebugger;
    OpticalFlow* opticalFlow;

    void InitBodyTest(Hal *hal, Config* config, VisualDebugger* visualDebugger) override {
        this->hal = hal;
        this->visualDebugger = visualDebugger;
        opticalFlow = new OpticalFlow();
    }

    bool BodyTestStep(double deltaTime) override {

        std::shared_ptr<cv::Mat> frame = hal->getFrame(Camera::Front);
        if(frame != NULL && !frame->empty()) {
            opticalFlow->Update(frame, deltaTime);
            visualDebugger->OpticalFlow(opticalFlow->GetPoints());
        }

        return true;

    }

    void FinishBodyTest() override {
        if(hal->getState() != State::Landed)
            hal->land();

        delete opticalFlow;
    }

};