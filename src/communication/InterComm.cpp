#include <chrono>
#include "InterComm.h"

#include "../messages/IpResolver.h"
#include "../messages/MessageBuilder.h"
#include "../config/ConfigKeys.h"

//Public

InterComm::InterComm() {

    messsageHandler.registerHandler(Message_Type::Message_Type_DRONESTATE,
                                    [this](Message m){ this->stateHandler(m);});
}

void InterComm::setupInterComm(Config* config){

    stateSendLapse = config->Get(ConfigKeys::Communications::AdvertisementLapse);

    broadcaster.setup(config->Get(ConfigKeys::Communications::BroadcastPort));

    this->name = config->Get(ConfigKeys::Drone::Name);
    this->id = config->Get(ConfigKeys::Drone::Id);
    socketPort = config->Get(ConfigKeys::Communications::CommunicationPort);

    IpResolver resolver;
    ip = resolver.resolve();

}

void InterComm::interCommStep(long runningTime, long deltaTime) {

    handleMessages();
    sendState(runningTime);
}

void InterComm::shutdownInterComm() {

}

//Private

void InterComm::stateHandler(Message &msg){

    DroneState* state = msg.mutable_dronestate();

    boost::asio::ip::address_v4 address = boost::asio::ip::address_v4(state->ip());

    if(address != ip || state->port() != socketPort){
        Logger::logDebug("State received from %s:%u with seq_num %u") << address.to_string() << state->port() << state->seq_num();

    }

}

void InterComm::handleMessages(){

    if(broadcaster.messageAvailable()) {
        Message msg = broadcaster.receive();
        messsageHandler.handle(msg);
    }

}

void InterComm::sendState(long runningTime) {

    if(runningTime - lastStateSend > stateSendLapse * 1000) {

        Message msg = MessageBuilder::build(Message_Type::Message_Type_DRONESTATE);

        DroneState* state = msg.mutable_dronestate();

        state->set_ip((uint32_t) ip.to_ulong());
        state->set_port(socketPort);
        state->set_drone_id(id);
        state->set_name(name);

        auto now = std::chrono::high_resolution_clock::now();
        seqNum = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count();
        state->set_seq_num(seqNum);

        broadcaster.broadcast(msg);
        Logger::logDebug("State sent");

        lastStateSend = runningTime;
    }
}


