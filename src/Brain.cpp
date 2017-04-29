//
// Created by bruno on 09/04/17.
//

#include <src/proto/message.pb.h>
#include <src/messages/MessageBuilder.h>
#include <src/messages/MessageBuilder.h>
#include <src/messages/Broadcaster.h>
#include "Brain.h"
#include "logging/Logger.h"

namespace chrono = std::chrono;

Brain::Brain() {
    messsageHandler.registerHandler(Message_Type::Message_Type_PING, [this](Message m){this->PingHandler(m);});
}

void Brain::setup(bool isRoot) {
    this->isRoot = isRoot;
    Logger::getInstance().setSource("BRAIN");

    if(!isRoot) {
        Broadcaster broadcaster;
        broadcaster.setup(BROADCAST_PORT);
        Logger::logInfo("Waiting for advertisement");
        Message msg = broadcaster.receive();
        Logger::logInfo("Advertisement received");
    }

    communicateWithBody(BRAIN_SERVE_PORT);


};

void Brain::communicateWithBody(unsigned short port) {
    communication.serve(port);
    Logger::logInfo("Brain has established a connection!");

    Message ping = MessageBuilder::build(Message_Type_PING);
    ping.mutable_ping()->set_type(Ping_PingType::Ping_PingType_REQUEST);

    communication.send(ping);
    Logger::logInfo("PING REQUEST sent");

}

void Brain::loop() {
    chrono::steady_clock::time_point startTime = chrono::steady_clock::now();
    chrono::steady_clock::time_point lastTime = startTime;
    chrono::steady_clock::time_point newTime = startTime;

    while (true) {
        lastTime = newTime;
        newTime = chrono::steady_clock::now();

        deltaTime = std::chrono::duration_cast<std::chrono::milliseconds>(newTime - lastTime).count();
        runningTime = std::chrono::duration_cast<std::chrono::milliseconds>(newTime - startTime).count();

        if(communication.messageAvailable()) {
            Message msg = communication.receive();
            messsageHandler.handle(msg);
        }

        if(isRoot) {
            advertise();
        }

        if(should_exit) {
            break;
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
        //shutdown();
    }

}

void Brain::shutdown() {
    should_exit = true;
    communication.send(MessageBuilder::build(Message_Type_SHUTDOWN));
    Logger::logWarning("SHUTDOWN request sent");
}

void Brain::advertise() {

    if(runningTime - lastAdvertisementTime > ADVERTISEMENT_LAPSE) {

        Message msg = MessageBuilder::build(Message_Type::Message_Type_ADVERTISEMENT);

        Broadcaster broadcaster;
        broadcaster.setup(BROADCAST_PORT);
        broadcaster.broadcast(msg);
        Logger::logDebug("Advertising sent");

        lastAdvertisementTime = runningTime;
    }
}
