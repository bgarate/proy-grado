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
#include "src/bodytests/BodyTest.h"
#include "src/bodytests/BodyTest1.cpp"
#include "src/bodytests/BodyTest2.cpp"
#include "src/bodytests/FlightManeuver.cpp"

namespace chrono = std::chrono;

Body::Body(Hal *hal) {
    this->hal = hal;
    messsageHandler.registerHandler(Message_Type::Message_Type_PING, [this](Message m){this->PingHandler(m);});
    messsageHandler.registerHandler(Message_Type::Message_Type_SHUTDOWN, [this](Message m){this->ShutdownHandler(m);});
}

void Body::setup(Config* config) {
    Logger::getInstance().setSource("BODY");
    communicateWithBrain(config->getBrainHost(), config->getBrainPort());
    hal->Connect();
}
void Body::communicateWithBrain(std::string brainHost, unsigned short port) {

    communication.connect(brainHost, port);
    Logger::logInfo("Body has established a connection!");

}

void Body::loop() {

    BodyTest* bt = new FlightManeuver();
    //BodyTest* bt = new BodyTest2();
    bt->InitBodyTest(this->hal);
    Logger::logInfo("Body started");

    chrono::steady_clock::time_point startTime = chrono::steady_clock::now();
    chrono::steady_clock::time_point lastTime;
    chrono::steady_clock::time_point newTime = startTime;

    //sleep(3);
    while (true) {
        lastTime = newTime;
        newTime = chrono::steady_clock::now();

        deltaTime = std::chrono::duration_cast<std::chrono::microseconds>(newTime - lastTime).count();
        runningTime = std::chrono::duration_cast<std::chrono::microseconds>(newTime - startTime).count();

        if(communication.messageAvailable()) {
            Message msg = communication.receive();
            messsageHandler.handle(msg);
        }

        bool res = bt->BodyTestStep(deltaTime);

        if(!res)
            break;

        if(should_exit)
            break;

        usleep(1000);
    }
    Logger::logDebug("Finishing test");
    bt->FinishBodyTest();

}

void Body::PingHandler(Message& msg){

    Ping* ping = msg.mutable_ping();
    if(ping->type() == Ping_PingType_REQUEST) {
        Logger::logInfo("PING REQUEST received");
        ping->set_type(Ping_PingType_ACK);
        communication.send(msg);
        Logger::logInfo("PING ACK sent");
    } else {
        Logger::logInfo("PING ACK received");

    }

}

void Body::ShutdownHandler(Message& msg){

    //DoShutdown* shutdown = msg.mutable_shutdown();
    Logger::logWarning("SHUTDOWN requested");

    should_exit = true;

}

void Body::cleanup() {
    hal->Disconnect();
}


