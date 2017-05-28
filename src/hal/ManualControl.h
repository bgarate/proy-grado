//
// Created by santy on 27/05/17.
//

#ifndef PROY_GRADO_MANUALCONTROL_H
#define PROY_GRADO_MANUALCONTROL_H


#include "hal.hpp"
#include <thread>

class ManualControl {

public:
    ManualControl(Hal* hal);

    void run();

    bool stopped();

private:
    void deamon();

    Hal* hal;
    bool isstopped;
    std::thread* t;
};

#endif //PROY_GRADO_MANUALCONTROL_H
