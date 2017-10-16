//
// Created by bruno on 09/04/17.
//

#ifndef PROY_GRADO_BRAIN_H
#define PROY_GRADO_BRAIN_H


#include "communication/InterCommunication.h"
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

    Config* config;

    bool should_exit = false;

    void shutdown();

    long deltaTime = 0;
    long runningTime = 0;

    InterCommunication * interComm;

};


#endif //PROY_GRADO_BRAIN_H
