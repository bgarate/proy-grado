//
// Created by bruno on 09/04/17.
//

#ifndef PROY_GRADO_BRAIN_H
#define PROY_GRADO_BRAIN_H


#include "communication/Communication.h"

class Brain {
public:
    void communicate(unsigned short port);
private:
    Communication communication;

};


#endif //PROY_GRADO_BRAIN_H
