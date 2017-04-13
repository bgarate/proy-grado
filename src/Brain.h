//
// Created by bruno on 09/04/17.
//

#ifndef PROY_GRADO_BRAIN_H
#define PROY_GRADO_BRAIN_H


#include "communication/Communication.h"
#include "MessageHandler.h"

class Brain {
public:
    Brain();

    void communicate(unsigned short port);
private:
    Communication communication;
    MessageHandler messsageHandler;

    void PingHandler(Message &msg);

    void loop();
};


#endif //PROY_GRADO_BRAIN_H
