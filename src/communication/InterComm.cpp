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

    this->name = config->Get(ConfigKeys::Drone::Name);
    this->id = config->Get(ConfigKeys::Drone::Id);
    this->broadcastPort = config->Get(ConfigKeys::Communications::BroadcastPort);
    this->stateSendLapse = config->Get(ConfigKeys::Communications::StateSendLapse);
    this->stateExpireLapse = config->Get(ConfigKeys::Communications::StateExpireLapse);

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

    this->runningTime = runningTime;

    handleMessages();
    sendState();

    //exipre messages
    for (std::map<int, long>::iterator it=droneTimestamps.begin(); it!=droneTimestamps.end(); ++it) {
        if (runningTime - it->second > stateExpireLapse * 1000) {
            droneStates.erase(it->first);
            droneTimestamps.erase(it->first);
        }
    }
}

void InterComm::shutdownInterComm() {

}

//Private

void InterComm::copyDroneState(DroneState* orig, DroneState* copy){

    copy->set_ip(orig->ip());
    copy->set_port(orig->port());
    copy->set_drone_id(orig->drone_id());
    copy->set_name(orig->name());
    copy->set_curren_task(orig->curren_task());
    copy->set_seq_num(orig->seq_num());
}

void InterComm::stateHandler(Message &msg){

    DroneState* state = msg.mutable_dronestate();

    int droneId = state->drone_id();
    long seqNum = state->seq_num();

    if(droneId != id && (lastSeq.find(droneId) != lastSeq.end() || lastSeq[droneId] < seqNum)){

        Logger::logDebug("State received from drone %u with seq_num %u") << droneId << seqNum;

        lastSeq[droneId] = seqNum;

        if (droneStates[droneId] == NULL) {
            droneStates[droneId] = new DroneState();
        }
        copyDroneState(state,droneStates[droneId]);
        droneTimestamps[droneId] = this->runningTime;
    }

}

void InterComm::handleMessages(){

    if(broadcaster.messageAvailable()) {
        Message msg = broadcaster.receive();
        messsageHandler.handle(msg);
    }

}

void InterComm::sendState() {

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


