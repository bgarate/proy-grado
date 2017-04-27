//
// Created by bruno on 09/04/17.
//

#include <src/proto/message.pb.h>
#include "Body.h"
#include "logging/Logger.h"
#include "Brain.h"


Body::Body(Hal *hal) {
    this->hal = hal;
    messsageHandler.registerHandler(Message_Type::Message_Type_PING, [this](Message m){this->PingHandler(m);});
}

void Body::communicate(std::string brainHost, unsigned short port) {

    communication.connect(brainHost, port);
    Logger::logInfo("Body has established a connection!");

}

void Body::loop() {
    while (true) {

        if(communication.messageAvailable()) {
            Message msg = communication.receive();
            messsageHandler.handle(msg);
        }

        sleep(0);
    }
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
