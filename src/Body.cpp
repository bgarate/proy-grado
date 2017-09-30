//
// Created by bruno on 09/04/17.
//

#include <src/proto/message.pb.h>
#include <chrono>
#include <src/bodytests/BodyTestRegistry.h>
#include "Body.h"
#include "logging/Logger.h"
#include "Brain.h"
#include "tracking/DetectAndTrack.h"
#include "tracking/MultiTracker.h"
#include "tracking/HogDetector.h"
#include "bodytests/BodyTest.h"
#include "bodytests/OpticalFlowObstacleAvoidance.cpp"
#include "bodytests/TrackMarkers.cpp"
#include "bodytests/BodyTest1.cpp"
#include "bodytests/BodyTest2.cpp"
#include "bodytests/FlightManeuver.cpp"
#include "bodytests/Follow.cpp"
#include "bodytests/PatrolAndFollow.cpp"
#include "bodytests/BodyTestRmove.cpp"
#include "bodytests/BodyTestRmove2.cpp"
#include "bodytests/BodyTestDummy.cpp"
#include "bodytests/BodyTestMarker.cpp"
#include "ConfigKeys.h"

Body::Body(Hal *hal) {
    this->hal = hal;
    messsageHandler.registerHandler(Message_Type::Message_Type_PING, [this](Message m){this->PingHandler(m);});
    messsageHandler.registerHandler(Message_Type::Message_Type_SHUTDOWN, [this](Message m){this->ShutdownHandler(m);});
}

void Body::setup(Config* config) {
    Logger::getInstance().setSource("BODY");
    this->config = config;
    communicateWithBrain(config->Get(ConfigKeys::Communications::BrainHost), config->Get(ConfigKeys::Communications::BrainPort));
    visualDebugger.setup(config);
    hal->setup(config);
    hal->Connect();
    pingWait = 0;

    this->mc = new ManualControl(hal);
    this->inmc = false;
}

void Body::communicateWithBrain(std::string brainHost, unsigned short port) {

    communication.connect(brainHost, port);
    Logger::logInfo("Body has established a connection!");

}

void Body::loop() {

    PrintAvailableTests();

    BodyTest* bt = BodyTestRegistry::Get(config->Get(ConfigKeys::Body::TestToExecute));

    bt->InitBodyTest(this->hal, config, &visualDebugger);
    Logger::logInfo("Body started");

    std::chrono::steady_clock::time_point startTime = std::chrono::steady_clock::now();
    std::chrono::steady_clock::time_point lastTime;
    std::chrono::steady_clock::time_point newTime = startTime;

    while (true) {
        lastTime = newTime;
        newTime = std::chrono::steady_clock::now();

        deltaTime = std::chrono::duration_cast<std::chrono::microseconds>(newTime - lastTime).count();
        runningTime = std::chrono::duration_cast<std::chrono::microseconds>(newTime - startTime).count();

        if(communication.messageAvailable()) {
            Message msg = communication.receive();
            messsageHandler.handle(msg);
        }

        if(config->Get(ConfigKeys::Communications::PingEnabled))
            waitPing();

        std::shared_ptr<cv::Mat> frame = hal->getFrame(Camera::Front);

        if (frame != NULL) {
            visualDebugger.setFrame(frame);
        }

        if(!inmc){

            bool res = bt->BodyTestStep(deltaTime);

            if(!res)
                should_exit = true;

            CalculateFPS();

            visualDebugger.setStatus(hal->getState(),hal->bateryLevel(),
                                     hal->getAltitude(), hal->getGPSPosition(), hal->getOrientation(), fps, runningTime);
            visualDebugger.drawMouse(deltaTime);
            visualDebugger.drawOrbSlam();
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
        } else if(mc->stopped()) {//q dentro de manual control
            should_exit = true;
        }


        if(should_exit)
            break;

        usleep(config->Get(ConfigKeys::Body::SleepDelay));
    }

    visualDebugger.cleanup();

    Logger::logDebug("Finishing test");
    bt->FinishBodyTest();

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

void Body::PingHandler(Message& msg){

    Ping* ping = msg.mutable_ping();
    if(ping->type() == Ping_PingType_REQUEST) {
        Logger::logDebug("PING REQUEST received. Last was %u milliseconds ago") << pingWait / 1000;
        ping->set_type(Ping_PingType_ACK);
        communication.send(msg);
        Logger::logDebug("PING ACK sent");
        pingWait = 0;
    } else {
        Logger::logDebug("PING ACK received");
    }

}

void Body::ShutdownHandler(Message& msg){

    //DoShutdown* shutdown = msg.mutable_shutdown();
    Logger::logWarning("SHUTDOWN requested");

    should_exit = true;

}

void Body::cleanup() {
    visualDebugger.cleanup();
    hal->Disconnect();
}

void Body::waitPing() {
    pingWait += deltaTime;

    if(pingWait > (config->Get(ConfigKeys::Communications::PingLapse) + config->Get(ConfigKeys::Communications::PingTimeout)) * 1000) {
        Logger::logError("Ping not received in %u milliseconds") << (int)(pingWait / 1000);
        should_exit = true;
    }
}

void Body::PrintAvailableTests() {

    std::string str = "Available body tests:\n";

    for(std::string& test : BodyTestRegistry::GetAvailableTests()){
        str += "\t- " + test + "\n";
    }

    Logger::logDebug(str);
}



