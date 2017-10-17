//
// Created by santy on 16/10/17.
//

#include "../config/ConfigKeys.h"
#include "BodyCommunication.h"


//public:

BodyCommunication::BodyCommunication(){

    messsageHandler.registerHandler(Message_Type::Message_Type_PING, [this](Message m){this->PingHandler(m);});
    messsageHandler.registerHandler(Message_Type::Message_Type_SHUTDOWN, [this](Message m){this->ShutdownHandler(m);});
}

void BodyCommunication::setupBodyComm(Config *config){

    bodySocket.connect(config->Get(ConfigKeys::Communications::BrainHost), config->Get(ConfigKeys::Communications::BrainPort));
    Logger::logInfo("Body has established a connection!");

    pingWait = 0;
    pingEnabled = config->Get(ConfigKeys::Communications::PingEnabled);
    pingTimeout = config->Get(ConfigKeys::Communications::PingTimeout);
    pingLapse = config->Get(ConfigKeys::Communications::PingLapse);
}

void BodyCommunication::bodyCommStep(long runningTime, long deltaTime){

    if(bodySocket.messageAvailable()) {
        Message msg = bodySocket.receive();
        messsageHandler.handle(msg);
    }

    if(pingEnabled)
        waitPing(deltaTime);

}

void BodyCommunication::shutdownBodyComm(){

}

bool BodyCommunication::shouldExit(){
    return should_exit;
}

//private:

void BodyCommunication::PingHandler(Message& msg){

    Ping* ping = msg.mutable_ping();
    if(ping->type() == Ping_PingType_REQUEST) {
        Logger::logDebug("PING REQUEST received. Last was %u milliseconds ago") << pingWait / 1000;
        ping->set_type(Ping_PingType_ACK);
        bodySocket.send(msg);
        Logger::logDebug("PING ACK sent");
        pingWait = 0;
    } else {
        Logger::logDebug("PING ACK received");
    }

}

void BodyCommunication::ShutdownHandler(Message& msg){

    //DoShutdown* shutdown = msg.mutable_shutdown();
    Logger::logWarning("SHUTDOWN requested");

    should_exit = true;

}

void BodyCommunication::waitPing(long deltaTime) {
    pingWait += deltaTime;

    if(pingWait > (pingLapse + pingTimeout) * 1000) {
        Logger::logError("Ping not received in %u milliseconds") << (int)(pingWait / 1000);
        //should_exit = true;
    }
}