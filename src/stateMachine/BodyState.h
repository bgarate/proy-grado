//
// Created by bruno on 04/03/18.
//

#ifndef PROY_GRADO_BODYSTATE_H
#define PROY_GRADO_BODYSTATE_H

#include "src/config/Config.h"
#include "BodyStateMachineControl.h"


class BodyState {
public:

    virtual std::string getName() = 0;

    virtual void init(Config* config, Hal* hal, SharedMemory* shared, BodyStateMachineControl contro,  VisualDebugger* visualDebugger, NavigationDebugger* navigationDebugger) = 0;

    virtual void prepare() = 0;

    virtual void step(double deltaTime) = 0;

    virtual void leave() = 0;

    virtual void cleanup() = 0;

};


#endif //PROY_GRADO_BODYSTATE_H
