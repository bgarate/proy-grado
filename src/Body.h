//
// Created by bruno on 09/04/17.
//

#ifndef PROY_GRADO_BODY_H
#define PROY_GRADO_BODY_H

#include <iostream>
#include "communication/Communication.h"

class Body {
public:
    void communicate(std::string brainHost, unsigned short port);
private:
    Communication communication;
};


#endif //PROY_GRADO_BODY_H
