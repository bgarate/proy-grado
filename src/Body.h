//
// Created by bruno on 09/04/17.
//

#ifndef PROY_GRADO_BODY_H
#define PROY_GRADO_BODY_H

#include <iostream>
#include "communication/Communication.h"
#include "MessageHandler.h"
#include "hal/hal.hpp"

class Body {
public:
    Body(Hal* hal);
    void communicate(std::string brainHost, unsigned short port);

    void loop();

private:
    Communication communication;
    MessageHandler messsageHandler;
    Hal* hal;
    void PingHandler(Message &msg);

};


#endif //PROY_GRADO_BODY_H
