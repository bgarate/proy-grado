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
    hal->setWhiteBalance(config->Get(ConfigKeys::Body::WhiteBalance));
    hal->setImageExposure(config->Get(ConfigKeys::Body::Exposure));
    hal->setImageSaturation(config->Get(ConfigKeys::Body::Saturation));

    this->mc = new ManualControl(hal);
    this->inmc = false;

    //bodyComm = new BodyComm();
    //bodyComm->setupBodyComm(config);

    StateMachine = new BodyStateMachine();
    Systems = new SystemManager();


}


void Body::loop() {

    std::string textToExecute = config->Get(ConfigKeys::Body::TestToExecute);

    Systems->Init(config, hal,shared, &visualDebugger, &navigationDebugger);

    if(config->Get(ConfigKeys::Body::VirtualBody))
        StateMachine->Init(StepName::VIRTUAL_DRONE,config,hal, shared, &visualDebugger, &navigationDebugger, Systems);
    else
        StateMachine->Init(StepName::TAKING_OFF,config,hal, shared, &visualDebugger, &navigationDebugger, Systems);

    Logger::logInfo("Body started");

    std::chrono::steady_clock::time_point startTime = std::chrono::steady_clock::now();
    std::chrono::steady_clock::time_point lastTime;
    std::chrono::steady_clock::time_point newTime = startTime;

    std::vector<VisualParameter> parameters = {
        VisualParameter(VisualParameterType::Saturation, "SAT", config->Get(ConfigKeys::Body::Saturation),
                        -100, 100, 10),
        VisualParameter(VisualParameterType::Exposure, "EXP", config->Get(ConfigKeys::Body::Exposure), -100, 100, 10),
        VisualParameter(VisualParameterType::WhiteBalance, "WHI", (int)config->Get(ConfigKeys::Body::WhiteBalance),
                        0, (int)WhiteBalanceMode::CoolWhite, 1),
        VisualParameter(VisualParameterType::Tilt, "TLT", config->Get(ConfigKeys::Drone::CameraTilt),
                        -config->Get(ConfigKeys::Drone::VerticalFOV) / 2,
                        config->Get(ConfigKeys::Drone::VerticalFOV) / 2, 0.5)
    };

    visualDebugger.setParameters(parameters);

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

        ProcessInput(key);
        ProcessParameters(visualDebugger.getParameters());

        BrainInfo brainInfo = shared->getBrainInfo();

        if(brainInfo.currentTask == BrainInfo::SHUTDOWN)
            should_exit = true;

        if(StateMachine->ShouldShutdown())
            should_exit = true;

        if(should_exit){
            BodyInfo bodyInfo = shared->getBodyInfo();
            bodyInfo.isShutingDown = true;
            shared->setBodyInfo(bodyInfo);
            break;
        }

        usleep(config->Get(ConfigKeys::Body::SleepDelay));
    }

    hal->land();

    while (hal->getState() != State::Landed)
        ;


}

void Body::ProcessInput(int key) {
    switch (key) {
        case 27:
            should_exit = true;
            break;
        case 32:
            mc->run();
            inmc = true;
            break;
        case (int) 'c':
            visualDebugger.captureImage();
            break;
    }
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
    Logger::logInfo("Cleaning up");

    visualDebugger.cleanup();
    StateMachine->Cleanup();
    Systems->Cleanup();
    hal->Disconnect();
}

void Body::ProcessParameters(std::vector<VisualParameter> parameters) {
    for(VisualParameter& parameter : parameters){
        if(!parameter.Changed)
            continue;

        switch (parameter.Type){
            case VisualParameterType::Tilt:
                config->Set(ConfigKeys::Drone::CameraTilt, (double)parameter.Value);
                break;
            case VisualParameterType::Exposure:
                config->Set(ConfigKeys::Body::Exposure, parameter.Value);
                hal->setImageExposure(parameter.Value);
                break;
            case VisualParameterType::Saturation:
                config->Set(ConfigKeys::Body::Saturation, parameter.Value);
                hal->setImageSaturation(parameter.Value);
                break;
            case VisualParameterType::WhiteBalance:
                WhiteBalanceMode  wb = (WhiteBalanceMode)(int)parameter.Value;
                config->Set(ConfigKeys::Body::WhiteBalance, wb);
                hal->setWhiteBalance(wb);
                break;
        }

    }
}
