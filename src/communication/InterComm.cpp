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

    this->name = config->Get(ConfigKeys::Drone::Name);
    this->id = config->Get(ConfigKeys::Drone::Id);
    broadcastPort = config->Get(ConfigKeys::Communications::BroadcastPort);

    broadcaster.setup(broadcastPort);

    IpResolver resolver;
    ip = resolver.resolve();

    droneStates[id] =  new DroneState();
    droneStates[id]->set_ip((uint32_t) ip.to_ulong());
    droneStates[id]->set_port(broadcastPort);
    droneStates[id]->set_drone_id(id);
    droneStates[id]->set_name(name);
    droneStates[id]->set_curren_task(DroneState::CurrentTask::DroneState_CurrentTask_INNACTIVE);

    auto now = std::chrono::high_resolution_clock::now();
    seqNum = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count();
    droneStates[id]->set_seq_num(seqNum);

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

    //boost::asio::ip::address_v4 address = boost::asio::ip::address_v4(state->ip());
    int droneId = state->drone_id();
    long seqNum = state->seq_num();

    if(droneId != id && (lastSeq.find(droneId) != lastSeq.end() || lastSeq[droneId] < seqNum)){

        Logger::logDebug("State received from drone %u with seq_num %u") << droneId << seqNum;

        lastSeq[droneId] = seqNum;
        droneStates[droneId] = state;

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

        state->set_ip(droneStates[id]->ip());
        state->set_port(droneStates[id]->port());
        state->set_drone_id(droneStates[id]->drone_id());
        state->set_name(droneStates[id]->name());
        state->set_curren_task(droneStates[id]->curren_task());

        auto now = std::chrono::high_resolution_clock::now();
        seqNum = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count();
        state->set_seq_num(seqNum);

        broadcaster.broadcast(msg);
        Logger::logDebug("State sent");

        lastStateSend = runningTime;
    }
}


