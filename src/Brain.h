//
// Created by bruno on 09/04/17.
//

#ifndef PROY_GRADO_BRAIN_H
#define PROY_GRADO_BRAIN_H


#include <src/communication/Communication.h>
#include <src/messages/SocketChannel.h>
#include <src/messages/Broadcaster.h>
#include "MessageHandler.h"
#include "Config.h"

class Brain {
public:
    Brain();

    void loop();

    void setup(Config* config);

    void cleanup();

private:
    Communication communication;
    SocketChannel bodyCommunication;
    MessageHandler messsageHandler;

    Config* config;
    void PingHandler(Message &msg);

    void advertise();

    bool should_exit = false;

    void communicateWithBody(unsigned short port);

    void shutdown();

    long deltaTime = 0;
    long runningTime = 0;
    long lastAdvertisementTime = 0;
    Broadcaster broadcaster;

    void handleMessages();

    void AdvertisementHandler(Message &msg);

    void HelloHandler(Message &msg);

    void sendPingIfAppropiate();

    unsigned int pingWait;
    bool waitingPing;
};


#endif //PROY_GRADO_BRAIN_H
