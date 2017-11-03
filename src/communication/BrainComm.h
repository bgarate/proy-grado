//
// Created by santy on 16/10/17.
//

#ifndef PROY_GRADO_INTRACOMMUNICATION_H
#define PROY_GRADO_INTRACOMMUNICATION_H


#include "../messages/SocketChannel.h"
#include "../messages/MessageHandler.h"
#include "../config/Config.h"

class BrainComm {

public:

    BrainComm();

    void setupBrainComm(Config *config);

    void brainCommStep(long runningTime, long deltaTime);

    void shutdownBrainComm();

private:

    SocketChannel brainSocket;
    MessageHandler messsageHandler;
    unsigned int pingWait;
    bool waitingPing;
    int pingTimeout;
    bool pingEnabled;


    void pingHandler(Message &msg);

    void handleMessages();

    void sendPingIfAppropiate(long deltaTime);

};


#endif //PROY_GRADO_INTRACOMMUNICATION_H
