//
// Created by bruno on 09/04/17.
//

#ifndef PROY_GRADO_BRAIN_H
#define PROY_GRADO_BRAIN_H


#include "communication/Communication.h"
#include "messages/SocketChannel.h"
#include "messages/Broadcaster.h"
#include "src/messages/MessageHandler.h"
#include "src/config/Config.h"

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
