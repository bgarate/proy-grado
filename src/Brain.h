//
// Created by bruno on 09/04/17.
//

#ifndef PROY_GRADO_BRAIN_H
#define PROY_GRADO_BRAIN_H


#include "communication/InterComm.h"
#include "communication/BrainComm.h"
#include "messages/SocketChannel.h"
#include "messages/Broadcaster.h"
#include "messages/MessageHandler.h"
#include "config/Config.h"
#include <src/debugging/MapDebugger.h>

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


    InterComm * interComm;
    BrainComm * brainComm;

    unsigned int myid;


    long lastDebug = 0;
    void debugDroneStates(long runningTime);

    //NAVDEB
    MapDebugger* mapDebugger;
    World world;

    Path path;
    int size;
    int nextMarker, previousMarker;

    long lastRefreshTime = 0;
    long pirntLapse = 0.1 * 1000 * 1000;
    long lastChange = 0;
    long lapseToChange = 2 * 1000 * 1000;
    //NAVDEB
};


#endif //PROY_GRADO_BRAIN_H
