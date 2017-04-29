//
// Created by bruno on 09/04/17.
//

#ifndef PROY_GRADO_BODY_H
#define PROY_GRADO_BODY_H

#include <iostream>
#include "src/messages/SocketChannel.h"
#include "MessageHandler.h"
#include "hal/hal.hpp"

class Body {
public:
    Body(Hal* hal);

    void setup(std::string brainHost);
    void loop();

private:
    SocketChannel communication;
    MessageHandler messsageHandler;
    Hal* hal;
    void PingHandler(Message &msg);
    void communicateWithBrain(std::string brainHost, unsigned short port);

    bool should_exit = false;

    void ShutdownHandler(Message &msg);
};


#endif //PROY_GRADO_BODY_H
