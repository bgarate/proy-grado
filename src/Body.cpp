//
// Created by bruno on 09/04/17.
//

#include <src/proto/message.pb.h>
#include <chrono>
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

Body::Body(Hal *hal) {
    this->hal = hal;
    messsageHandler.registerHandler(Message_Type::Message_Type_PING, [this](Message m){this->PingHandler(m);});
    messsageHandler.registerHandler(Message_Type::Message_Type_SHUTDOWN, [this](Message m){this->ShutdownHandler(m);});
}

void Body::setup(Config* config) {
    Logger::getInstance().setSource("BODY");
    this->config = config;
    communicateWithBrain(config->getBrainHost(), config->getBrainPort());
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

    BodyTest* bt = new PatrolAndFollow();
    //BodyTest* bt = new OrbSlam2();
    //BodyTest* bt = new OpticalFlowObstacleAvoidance();
    //BodyTest* bt = new BodyTestRmove2();
    //BodyTest* bt = new Follow();
    //BodyTest* bt = new BodyTestMarker();
    //BodyTest* bt = new TrackMarkers();

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

        if(config->isPingEnabled())
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
                config->setCameraTilt(std::min(config->getVerticalFov()/2,config->getCameraTilt() + 0.1));
            } else if (key == (int)'r'){
                config->setCameraTilt(std::max(-config->getVerticalFov()/2,config->getCameraTilt() - 0.1));
            }

        } else if(mc->stopped()) {//q dentro de manual control
            should_exit = true;
        }


        if(should_exit)
            break;

        usleep(config->getSleepDelay());
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

    if(pingWait > (config->getPingLapse() + config->getPingTimeout()) * 1000) {
        Logger::logError("Ping not received in %u milliseconds") << (int)(pingWait / 1000);
        should_exit = true;
    }
}



