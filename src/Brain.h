//
// Created by bruno on 09/04/17.
//

#ifndef PROY_GRADO_BRAIN_H
#define PROY_GRADO_BRAIN_H


#include "communication/InterComm.h"
#include "config/Config.h"
#include "debugging/MapDebugger.h"

class Brain {
public:
    Brain();

    void loop();

    void setup(Config* config, SharedMemory* shared);

    void cleanup();

private:

    Config* config;
    SharedMemory *shared;

    bool should_exit = false;

    void shutdown();

    long deltaTime = 0;
    long runningTime = 0;

    long currentFlyingTime = 0;
    long MAX_FLYING_TIME = 20000000;

    InterComm * interComm;

    unsigned int myid;
    unsigned int alertid;

    int lowBatteryLevel;
    int critialBatteryLevel;

    //Movimiento simulado
    std::map<int, Path> paths;

    int closestPad = -1;
    std::vector<WorldObject *> pads;

    //Map debugger
    MapDebugger* mapDebugger;
    bool mapEnabled;
    World world;
    long lastRefreshTime = 0;
    long pirntLapse = 0.05 * 1000 * 1000;

    DroneState_CurrentTask MapCurrentTask(BrainInfo::CurrentTask task);
};


#endif //PROY_GRADO_BRAIN_H
