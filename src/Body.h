//
// Created by bruno on 09/04/17.
//

#ifndef PROY_GRADO_BODY_H
#define PROY_GRADO_BODY_H

#include <iostream>
#include "communication/BodyComm.h"
#include "hal/ManualControl.h"
#include "messages/SocketChannel.h"
#include "messages/MessageHandler.h"
#include "hal/hal.hpp"
#include "config/Config.h"
#include "debugging/VisualDebugger.h"

class Body {
public:
    Body(Hal* hal);

    void setup(Config* config);
    void loop();
    void cleanup();
private:

    VisualDebugger visualDebugger;
    Hal* hal;

    //BodyComm * bodyComm;

    bool should_exit = false;
    Config* config;

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
