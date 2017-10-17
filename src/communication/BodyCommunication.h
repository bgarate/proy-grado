//
// Created by santy on 16/10/17.
//

#ifndef PROY_GRADO_BODYCOMMUNICATION_H
#define PROY_GRADO_BODYCOMMUNICATION_H


#include "../messages/SocketChannel.h"
#include "../messages/MessageHandler.h"
#include "../config/Config.h"

class BodyCommunication {

public:

    BodyCommunication();

    void setupBodyComm(Config *config);

    void bodyCommStep(long runningTime, long deltaTime);

    bool shouldExit();

    void shutdownBodyComm();

private:

    SocketChannel bodySocket;
    MessageHandler messsageHandler;
    long pingWait;
    bool pingEnabled;
    int pingTimeout;
    int pingLapse;
    bool should_exit;

    void PingHandler(Message &msg);

    void communicateWithBrain(std::string brainHost, unsigned short port);

    void ShutdownHandler(Message &msg);

    void waitPing(long deltaTime);

};


#endif //PROY_GRADO_BODYCOMMUNICATION_H
