//
// Created by bruno on 09/04/17.
//

#ifndef PROY_GRADO_BRAIN_H
#define PROY_GRADO_BRAIN_H


#include "communication/InterCommunication.h"
#include "communication/BrainCommunication.h"
#include "messages/SocketChannel.h"
#include "messages/Broadcaster.h"
#include "messages/MessageHandler.h"
#include "config/Config.h"

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
    BrainCommunication * brainComm;

};


#endif //PROY_GRADO_BRAIN_H
