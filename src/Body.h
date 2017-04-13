//
// Created by bruno on 09/04/17.
//

#ifndef PROY_GRADO_BODY_H
#define PROY_GRADO_BODY_H

#include <iostream>
#include "communication/Communication.h"
#include "MessageHandler.h"

class Body {
public:
    Body();
    void communicate(std::string brainHost, unsigned short port);
private:
    Communication communication;
    MessageHandler messsageHandler;

    void PingHandler(Message &msg);

    void loop();
};


#endif //PROY_GRADO_BODY_H
