//
// Created by bruno on 09/04/17.
//

#include <src/proto/message.pb.h>
#include <src/communication/MessageBuilder.h>
#include "Brain.h"
#include "logging/Logger.h"

Brain::Brain() {
    messsageHandler.registerHandler(Message_Type::Message_Type_PING, [this](Message m){this->PingHandler(m);});
}

void Brain::communicate(unsigned short port) {
    communication.serve(port);
    Logger::logInfo("Brain has established a connection!");

    Message ping = MessageBuilder::build(Message_Type_PING);
    ping.mutable_ping()->set_type(Ping_PingType::Ping_PingType_REQUEST);

    communication.send(ping);
    Logger::logInfo("PING REQUEST sent");

    loop();

}

void Brain::loop() {
    while (true) {

        if(communication.messageAvailable()) {
            Message msg = communication.receive();
            messsageHandler.handle(msg);
        }

        sleep(0);
    }
}

void Brain::PingHandler(Message& msg){

    Ping* ping = msg.mutable_ping();
    if(ping->type() == Ping_PingType_REQUEST) {
        Logger::logInfo("PING REQUEST received");
        ping->set_type(Ping_PingType_ACK);
        communication.send(msg);
    } else {
        Logger::logInfo("PING ACK received");
    }

}