//
// Created by bruno on 09/04/17.
//

#include <src/proto/message.pb.h>
#include <src/messages/MessageBuilder.h>
#include <src/messages/Broadcaster.h>
#include "Brain.h"
#include "logging/Logger.h"
#include "Config.h"

namespace chrono = std::chrono;

Brain::Brain() {
    messsageHandler.registerHandler(Message_Type::Message_Type_PING,
                                    [this](Message m){this->PingHandler(m);});
    messsageHandler.registerHandler(Message_Type::Message_Type_ADVERTISEMENT,
                                    [this](Message m){this->AdvertisementHandler(m);});
    messsageHandler.registerHandler(Message_Type::Message_Type_HELLO,
                                    [this](Message m){this->HelloHandler(m);});
}

void Brain::setup(Config* config) {
    this->config = config;

    Logger::getInstance().setSource("BRAIN");

    communicateWithBody(config->getBrainPort());
    broadcaster.setup(config->getBroadcastPort());
    communication.setup(config->getName(), config->getId());
    communication.serve(config->getCommsPort());

    pingWait = 0;
    waitingPing = false;
};

void Brain::communicateWithBody(unsigned short port) {
    bodyCommunication.serve(port);
    Logger::logInfo("Brain has established a connection!");
}

void Brain::loop() {
    chrono::steady_clock::time_point startTime = chrono::steady_clock::now();
    chrono::steady_clock::time_point lastTime = startTime;
    chrono::steady_clock::time_point newTime = startTime;

    Logger::logInfo("Advertisement received");

    while (true) {
        lastTime = newTime;
        newTime = chrono::steady_clock::now();

        deltaTime = std::chrono::duration_cast<std::chrono::microseconds>(newTime - lastTime).count();
        runningTime = std::chrono::duration_cast<std::chrono::microseconds>(newTime - startTime).count();

        handleMessages();

        advertise();
        sendPingIfAppropiate();

        if(should_exit) {
            break;
        }

        sleep(0);
    }
}

void Brain::sendPingIfAppropiate() {

    pingWait += deltaTime;

    if(waitingPing && pingWait > config->getPingTimeout()  * 1000) {
        Logger::logError("Ping not ACKed in %u milliseconds") << pingWait / 1000;
        should_exit = true;
    } else if(!waitingPing && pingWait > config->getPingLapse() * 1000) {
        Message ping = MessageBuilder::build(Message_Type_PING);
        ping.mutable_ping()->set_type(Ping_PingType::Ping_PingType_REQUEST);

        bodyCommunication.send(ping);
        Logger::logDebug("PING REQUEST sent");

        pingWait = 0;
        waitingPing = true;
    }
}

void Brain::handleMessages() {
    if(bodyCommunication.messageAvailable()) {
        Message msg = bodyCommunication.receive();
        messsageHandler.handle(msg);
    }

    if(broadcaster.messageAvailable()) {
        Message msg = broadcaster.receive();
        messsageHandler.handle(msg);
    }

    while (communication.messageAvailable()) {
        Message msg = communication.getMessage();
        messsageHandler.handle(msg);
    }
}

void Brain::PingHandler(Message& msg){

    Ping* ping = msg.mutable_ping();
    if(ping->type() == Ping_PingType_REQUEST) {

        Logger::logDebug("PING REQUEST received");
        ping->set_type(Ping_PingType_ACK);
        bodyCommunication.send(msg);

    } else {
        Logger::logDebug("PING ACK received. Request was sent %u milliseconds ago") << pingWait / 1000;
        waitingPing = false;
        pingWait = 0;
    }

}

void Brain::shutdown() {
    should_exit = true;
    bodyCommunication.send(MessageBuilder::build(Message_Type_SHUTDOWN));
    Logger::logWarning("SHUTDOWN request sent");
}

void Brain::advertise() {

    if(runningTime - lastAdvertisementTime > config->getAdvertisementLapse() * 1000) {

        Message msg = MessageBuilder::build(Message_Type::Message_Type_ADVERTISEMENT);

        Advertisement* advertisement = msg.mutable_advertisement();

        advertisement->set_ip((uint32_t) communication.getIp().to_ulong());
        advertisement->set_port(communication.getPort());

        broadcaster.broadcast(msg);
        Logger::logDebug("Advertising sent");

        lastAdvertisementTime = runningTime;
    }
}

void Brain::AdvertisementHandler(Message& msg){

    Advertisement* advertisement = msg.mutable_advertisement();

    boost::asio::ip::address_v4 address = boost::asio::ip::address_v4(advertisement->ip());

    if(address != communication.getIp() && advertisement->port() != communication.getPort()){
        Logger::logDebug("Advertisement received from %s:%u") << address.to_string() <<
                         advertisement->port();

        communication.connect(address, (unsigned short) advertisement->port());
    }

}

void Brain::HelloHandler(Message& msg){

    Hello* hello = msg.mutable_hello();

    boost::asio::ip::address_v4 address = boost::asio::ip::address_v4(hello->ip());

    Logger::logDebug("Hello received from %s(%u) at %s:%u") << hello->name() <<
                     hello->id() << address.to_string() <<
                     hello->port();

    communication.asociate(hello);

}

void Brain::cleanup() {

}