//
// Created by bruno on 09/04/17.
//

#ifndef PROY_GRADO_BODY_H
#define PROY_GRADO_BODY_H

#include <iostream>
#include "hal/ManualControl.h"
#include "messages/SocketChannel.h"
#include "MessageHandler.h"
#include "hal/hal.hpp"
#include "Config.h"
#include "VisualDebugger.h"

class Body {
public:
    Body(Hal* hal);

    void setup(Config* config);
    void loop();
    void cleanup();
private:
    SocketChannel communication;
    MessageHandler messsageHandler;
    VisualDebugger visualDebugger;
    Hal* hal;
    void PingHandler(Message &msg);
    void communicateWithBrain(std::string brainHost, unsigned short port);

    bool should_exit = false;
    Config* config;
    void ShutdownHandler(Message &msg);

    int elapsedFrames = 0;
    int elapsedTime = 0;
    int fps = 0;

    int deltaTime = 0;
    long runningTime = 0;

    void waitPing();

    long pingWait;

    ManualControl *mc;
    bool inmc;

    void CalculateFPS();

    void PrintAvailableTests();
};


#endif //PROY_GRADO_BODY_H
