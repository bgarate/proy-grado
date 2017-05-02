//
// Created by bruno on 09/04/17.
//

#ifndef PROY_GRADO_BRAIN_H
#define PROY_GRADO_BRAIN_H


#include "src/messages/SocketChannel.h"
#include "src/messages/Broadcaster.h"
#include "MessageHandler.h"

class Brain {
public:
    Brain();

    void loop();

    void setup(bool isRoot);
    static const unsigned short BRAIN_SERVE_PORT = 11500;

private:
    SocketChannel communication;
    MessageHandler messsageHandler;

    void PingHandler(Message &msg);

    void advertise();

    bool should_exit = false;
    bool isRoot;
    static const unsigned short BROADCAST_PORT = 11501;

    void communicateWithBody(unsigned short port);

    void shutdown();

    long deltaTime = 0;
    long runningTime = 0;
    long lastAdvertisementTime = 0;
    static const long ADVERTISEMENT_LAPSE = 5000;
    Broadcaster broadcaster;
};


#endif //PROY_GRADO_BRAIN_H
