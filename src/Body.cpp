//
// Created by bruno on 09/04/17.
//

#include <src/proto/message.pb.h>
#include "Body.h"
#include "logging/Logger.h"
#include "Brain.h"
#include "tracking/DetectAndTrack.h"
#include "tracking/MultiTracker.h"
#include "tracking/HogDetector.h"
#include "BodyTest.h"
#include "BodyTest1.cpp"

Body::Body(Hal *hal) {
    this->hal = hal;
    messsageHandler.registerHandler(Message_Type::Message_Type_PING, [this](Message m){this->PingHandler(m);});
    messsageHandler.registerHandler(Message_Type::Message_Type_SHUTDOWN, [this](Message m){this->ShutdownHandler(m);});
}


void Body::setup(std::string brainHost) {
    Logger::getInstance().setSource("BODY");
    communicateWithBrain(brainHost, Brain::BRAIN_SERVE_PORT);
}
void Body::communicateWithBrain(std::string brainHost, unsigned short port) {

    communication.connect(brainHost, port);
    Logger::logInfo("Body has established a connection!");

}

void Body::loop() {

    BodyTest* bt = new BodyTest1();
    bt->InitBodyTest(this->hal);

    while (true) {
        if(communication.messageAvailable()) {
            Message msg = communication.receive();
            messsageHandler.handle(msg);
        }
        
        bt->BodyTestStep();

        if(should_exit)
            break;

        sleep(0);
    }
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


