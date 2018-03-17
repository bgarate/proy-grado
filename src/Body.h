//
// Created by bruno on 09/04/17.
//

#ifndef PROY_GRADO_BODY_H
#define PROY_GRADO_BODY_H

#include <iostream>
#include <src/navigation/NavigationDebugger.h>
#include <src/stateMachine/BodyStateMachine.h>
#include <src/systems/SystemManager.h>
#include "communication/BodyComm.h"
#include "hal/ManualControl.h"
#include "messages/SocketChannel.h"
#include "messages/MessageHandler.h"
#include "hal/hal.hpp"
#include "config/Config.h"
#include "debugging/VisualDebugger.h"

class Body {
public:
    Body(Hal *hal);

    void setup(Config *config, SharedMemory *shared);

    void loop();

    void cleanup();

    BodyStateMachine* StateMachine = NULL;
    SystemManager* Systems = NULL;

private:

    VisualDebugger visualDebugger;
    NavigationDebugger navigationDebugger;
    Hal *hal;


    BodyComm *bodyComm;
    bool should_exit = false;
    Config* config;
    SharedMemory* shared;

    int elapsedFrames = 0;
    int elapsedTime = 0;
    int fps = 0;

    int deltaTime = 0;
    long runningTime = 0;

    ManualControl *mc;
    bool inmc;

    void CalculateFPS();

    void PrintAvailableTests();
};


#endif //PROY_GRADO_BODY_H
