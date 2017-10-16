//
// Created by santy on 15/10/17.
//

#include "../messages/MessageBuilder.h"
#include "../config/ConfigKeys.h"
#include "InterCommunication.h"

//Public

InterCommunication::InterCommunication(){

    messsageHandler.registerHandler(Message_Type::Message_Type_PING,
                                    [this](Message m){this->pingHandler(m);});
    messsageHandler.registerHandler(Message_Type::Message_Type_ADVERTISEMENT,
                                    [this](Message m){this->advertisementHandler(m);});
    messsageHandler.registerHandler(Message_Type::Message_Type_HELLO,
                                    [this](Message m){this->helloHandler(m);});
}

void InterCommunication::setupInterComm(Config* config){

    communicateWithBody(config->Get(ConfigKeys::Communications::BrainPort));
    broadcaster.setup(config->Get(ConfigKeys::Communications::BroadcastPort));
    communication.setup(config->Get(ConfigKeys::Drone::Name), config->Get(ConfigKeys::Drone::Id));
    communication.serve(config->Get(ConfigKeys::Communications::CommunicationPort));

    advertisementLapse = config->Get(ConfigKeys::Communications::AdvertisementLapse);
    pingTimeout = config->Get(ConfigKeys::Communications::PingTimeout);
    pingEnabled = config->Get(ConfigKeys::Communications::PingEnabled);

    pingWait = 0;
    waitingPing = false;
}

void InterCommunication::interCommStep(long runningTime, long deltaTime) {

    handleMessages();

    advertise(runningTime);

    if(pingEnabled)
        sendPingIfAppropiate(deltaTime);
}

void InterCommunication::shutdownInterComm() {

    bodyCommunication.send(MessageBuilder::build(Message_Type_SHUTDOWN));
    Logger::logWarning("SHUTDOWN request sent");
}

//Private


void InterCommunication::pingHandler(Message& msg){

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

void InterCommunication::advertisementHandler(Message& msg){

    Advertisement* advertisement = msg.mutable_advertisement();

    boost::asio::ip::address_v4 address = boost::asio::ip::address_v4(advertisement->ip());

    std::cout << std::endl << "Manejando Advertise" << std::endl;

    if(address != communication.getIp() && advertisement->port() != communication.getPort()){
        Logger::logDebug("Advertisement received from %s:%u") << address.to_string() <<
                                                              advertisement->port();

        communication.connect(address, (unsigned short) advertisement->port());
    }

}

void InterCommunication::helloHandler(Message& msg){

    Hello* hello = msg.mutable_hello();

    boost::asio::ip::address_v4 address = boost::asio::ip::address_v4(hello->ip());

    Logger::logDebug("Hello received from %s(%u) at %s:%u") << hello->name() <<
                                                            hello->id() << address.to_string() <<
                                                            hello->port();

    communication.asociate(hello);

}

void InterCommunication::communicateWithBody(unsigned short port) {

    bodyCommunication.serve(port);
    Logger::logInfo("Brain has established a connection!");
}

void InterCommunication::handleMessages(){

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

void InterCommunication::advertise(long runningTime) {

    if(runningTime - lastAdvertisementTime > advertisementLapse * 1000) {

        Message msg = MessageBuilder::build(Message_Type::Message_Type_ADVERTISEMENT);

        Advertisement* advertisement = msg.mutable_advertisement();

        advertisement->set_ip((uint32_t) communication.getIp().to_ulong());
        advertisement->set_port(communication.getPort());

        broadcaster.broadcast(msg);
        Logger::logDebug("Advertising sent");

        lastAdvertisementTime = runningTime;
    }
}

void InterCommunication::sendPingIfAppropiate(long deltaTime) {

    pingWait += deltaTime;

    if(waitingPing && pingWait > pingTimeout * 1000) {

        Logger::logError("Ping not ACKed in %u milliseconds") << pingWait / 1000;
        //should_exit = true;

    } else if(!waitingPing && pingWait > pingTimeout * 1000) {

        Message ping = MessageBuilder::build(Message_Type_PING);
        ping.mutable_ping()->set_type(Ping_PingType::Ping_PingType_REQUEST);

        bodyCommunication.send(ping);
        Logger::logDebug("PING REQUEST sent");

        pingWait = 0;
        waitingPing = true;
    }
}
