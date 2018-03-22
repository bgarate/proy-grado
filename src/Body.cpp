//
// Created by bruno on 09/04/17.
//

#include <src/proto/message.pb.h>
#include <chrono>
#include <src/communication/SharedMemory.h>
#include <src/stateMachine/StepName.h>
#include "bodytests/BodyTestRegistry.h"
#include "Body.h"
#include "Brain.h"
#include "bodytests/TrackMarkers.cpp"

Body::Body(Hal *hal) {

    this->hal = hal;
}

void Body::setup(Config* config, SharedMemory* shared) {
    Logger::getInstance().setSource("BODY");

    this->config = config;
    this->shared = shared;

    visualDebugger.setup(config);
    //navigationDebugger.Init(config);
    hal->setup(config);
    hal->Connect();

    this->mc = new ManualControl(hal);
    this->inmc = false;

    //bodyComm = new BodyComm();
    //bodyComm->setupBodyComm(config);

    StateMachine = new BodyStateMachine();
    Systems = new SystemManager();


}


void Body::loop() {

    PrintAvailableTests();

    std::string textToExecute = config->Get(ConfigKeys::Body::TestToExecute);

    Systems->Init(config, hal,shared, &visualDebugger, &navigationDebugger);
    //StateMachine->Init(StepName::TAKING_OFF,config,hal, shared, &visualDebugger, &navigationDebugger);
    StateMachine->Init(StepName::VIRTUAL_DRONE,config,hal, shared, &visualDebugger, &navigationDebugger);

    Logger::logInfo("Body started");

    std::chrono::steady_clock::time_point startTime = std::chrono::steady_clock::now();
    std::chrono::steady_clock::time_point lastTime;
    std::chrono::steady_clock::time_point newTime = startTime;

    while (true) {
        lastTime = newTime;
        newTime = std::chrono::steady_clock::now();

        deltaTime = std::chrono::duration_cast<std::chrono::microseconds>(newTime - lastTime).count();
        runningTime = std::chrono::duration_cast<std::chrono::microseconds>(newTime - startTime).count();

        std::shared_ptr<cv::Mat> frame = hal->getFrame(Camera::Front);

        //bodyComm->bodyCommStep(runningTime, deltaTime);

        if (frame != NULL) {
            visualDebugger.setFrame(frame);
        }

        Systems->Step(deltaTime);
        StateMachine->Step(deltaTime);

        CalculateFPS();

        visualDebugger.setStatus(hal->getState(),hal->bateryLevel(),
                                 hal->getAltitude(), hal->getGPSPosition(), hal->getOrientation(), fps, runningTime);
        visualDebugger.drawMouse(deltaTime);

        BodyInfo info = shared->getBodyInfo();
        //navigationDebugger.Run(info.ExecutedCommand, info.CurrentTargetId, info.PredictedFuturePosition,
        //                        info.ProjectedPositionOnPath, info.TargetOnPath);

        int key = visualDebugger.show(deltaTime);

        if(key == 27){
            should_exit = true;
        } else if (key == 32) {
            visualDebugger.cleanup();
            mc->run();
            inmc=true;
        } else if (key == (int)'c'){
            visualDebugger.captureImage();
        } else if (key == (int)'t'){
            config->Set(ConfigKeys::Drone::CameraTilt,std::min(config->Get(ConfigKeys::Drone::VerticalFOV)/2,config->Get(ConfigKeys::Drone::CameraTilt) + 0.1));
        } else if (key == (int)'r'){
            config->Set(ConfigKeys::Drone::CameraTilt,std::max(config->Get(ConfigKeys::Drone::VerticalFOV)/2,config->Get(ConfigKeys::Drone::CameraTilt) - 0.1));
        }

        if(should_exit /*|| bodyComm->shouldExit()*/)

            break;

        usleep(config->Get(ConfigKeys::Body::SleepDelay));
    }

    visualDebugger.cleanup();

    Logger::logDebug("Finishing test");

    StateMachine->Cleanup();
    Systems->Cleanup();

}

void Body::CalculateFPS() {
    elapsedFrames++;
    elapsedTime += deltaTime;

    if (elapsedTime >= 1000000) {
        fps = elapsedFrames * 1000000 / elapsedTime;
        elapsedFrames = 0;
        elapsedTime = 0;
    }
}

void Body::cleanup() {
    visualDebugger.cleanup();
    hal->Disconnect();
}

void Body::PrintAvailableTests() {

    std::string str = "Available body tests:\n";

    for(std::string& test : BodyTestRegistry::GetAvailableTests()){
        str += "\t- " + test + "\n";
    }

    Logger::logDebug(str);
}



