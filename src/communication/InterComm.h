//
// Created by santy on 03/11/17.
//

#ifndef PROY_GRADO_INTERCOMM_H
#define PROY_GRADO_INTERCOMM_H

#include "../messages/Connection.h"
#include "../messages/AsyncSocketChannel.h"
#include "../config/Config.h"
#include "../messages/Broadcaster.h"
#include "../messages/SocketChannel.h"
#include "../messages/MessageHandler.h"

class InterComm {

public:

    InterComm();

    void setupInterComm(Config* config);

    void interCommStep(long runningTime, long deltaTime);

    void shutdownInterComm();

    std::map<int, DroneState*> droneStates;

private:

    std::map<int, long> lastSeq;

    long seqNum;

    std::string name;
    unsigned int id;
    int broadcastPort;
    boost::asio::ip::address_v4 ip;

    MessageHandler messsageHandler;
    long lastStateSend = 0;
    Broadcaster broadcaster;
    int stateSendLapse;

    void copyDroneState(DroneState* orig, DroneState* copy);

    void stateHandler(Message &msg);

    void sendState(long runningTime);

    void handleMessages();


};


#endif //PROY_GRADO_INTERCOMM_H
