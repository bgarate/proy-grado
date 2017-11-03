#include "InterComm.h"

#include "../messages/IpResolver.h"
#include "../messages/MessageBuilder.h"
#include "../config/ConfigKeys.h"

//Public

InterComm::InterComm() {

    messsageHandler.registerHandler(Message_Type::Message_Type_ADVERTISEMENT,
                                    [this](Message m){this->advertisementHandler(m);});
}

void InterComm::setupInterComm(Config* config){

    advertisementLapse = config->Get(ConfigKeys::Communications::AdvertisementLapse);

    broadcaster.setup(config->Get(ConfigKeys::Communications::BroadcastPort));

    this->name = config->Get(ConfigKeys::Drone::Name);
    this->id = config->Get(ConfigKeys::Drone::Id);
    socketPort = config->Get(ConfigKeys::Communications::CommunicationPort);

    IpResolver resolver;
    ip = resolver.resolve();

}

void InterComm::interCommStep(long runningTime, long deltaTime) {

    handleMessages();
    advertise(runningTime);
}

void InterComm::shutdownInterComm() {

}

//Private

void InterComm::advertisementHandler(Message& msg){

    Advertisement* advertisement = msg.mutable_advertisement();

    boost::asio::ip::address_v4 address = boost::asio::ip::address_v4(advertisement->ip());

    if(address != ip || advertisement->port() != socketPort){
        Logger::logDebug("Advertisement received from %s:%u") << address.to_string() <<
                                                              advertisement->port();

    }

}

void InterComm::handleMessages(){

    if(broadcaster.messageAvailable()) {
        Message msg = broadcaster.receive();
        messsageHandler.handle(msg);
    }

}

void InterComm::advertise(long runningTime) {

    if(runningTime - lastAdvertisementTime > advertisementLapse * 1000) {

        Message msg = MessageBuilder::build(Message_Type::Message_Type_ADVERTISEMENT);

        Advertisement* advertisement = msg.mutable_advertisement();

        advertisement->set_ip((uint32_t) ip.to_ulong());
        advertisement->set_port(socketPort);

        broadcaster.broadcast(msg);
        Logger::logDebug("Advertising sent");

        lastAdvertisementTime = runningTime;
    }
}


