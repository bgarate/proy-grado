//
// Created by santy on 16/10/17.
//

#include "BrainComm.h"
#include "../messages/MessageBuilder.h"
#include "../config/ConfigKeys.h"


//Public

BrainComm::BrainComm(){

    messsageHandler.registerHandler(Message_Type::Message_Type_PING,
                                    [this](Message m){this->pingHandler(m);});
}

void BrainComm::setupBrainComm(Config *config){

    brainSocket.serve(config->Get(ConfigKeys::Communications::BrainPort));
    Logger::logInfo("Brain has established a connection!");

    pingTimeout = config->Get(ConfigKeys::Communications::PingTimeout);
    pingEnabled = config->Get(ConfigKeys::Communications::PingEnabled);

    pingWait = 0;
    waitingPing = false;
}

void BrainComm::brainCommStep(long runningTime, long deltaTime) {

    handleMessages();

    if(pingEnabled)
        sendPingIfAppropiate(deltaTime);
}

void BrainComm::shutdownBrainComm() {

    brainSocket.send(MessageBuilder::build(Message_Type_SHUTDOWN));
    Logger::logWarning("SHUTDOWN request sent");
}

//Private

void BrainComm::sendPingIfAppropiate(long deltaTime) {

    pingWait += deltaTime;

    if(waitingPing && pingWait > pingTimeout * 1000) {

        Logger::logError("Ping not ACKed in %u milliseconds") << pingWait / 1000;
        //should_exit = true;

    } else if(!waitingPing && pingWait > pingTimeout * 1000) {

        Message ping = MessageBuilder::build(Message_Type_PING);
        ping.mutable_ping()->set_type(Ping_PingType::Ping_PingType_REQUEST);

        brainSocket.send(ping);
        Logger::logDebug("PING REQUEST sent");

        pingWait = 0;
        waitingPing = true;
    }
}

void BrainComm::pingHandler(Message& msg){

    Ping* ping = msg.mutable_ping();
    if(ping->type() == Ping_PingType_REQUEST) {

        Logger::logDebug("PING REQUEST received");
        ping->set_type(Ping_PingType_ACK);
        brainSocket.send(msg);

    } else {
        Logger::logDebug("PING ACK received. Request was sent %u milliseconds ago") << pingWait / 1000;
        waitingPing = false;
        pingWait = 0;
    }

}


void BrainComm::handleMessages(){

    if(brainSocket.messageAvailable()) {
        Message msg = brainSocket.receive();
        messsageHandler.handle(msg);
    }
}


