//
// Created by bruno on 30/03/18.
//

#ifndef PROY_GRADO_BODYSTATEBASE_H
#define PROY_GRADO_BODYSTATEBASE_H


#include <src/hal/hal.hpp>
#include <src/communication/SharedMemory.h>
#include <src/debugging/VisualDebugger.h>
#include <src/navigation/NavigationDebugger.h>
#include "IBodyState.h"

class BodyStateBase : public IBodyState {
public:
    bool shouldTransition();
    void doTransition();
    void init(Config* config, Hal* hal, SharedMemory* shared, BodyStateMachineControl* control,  VisualDebugger* visualDebugger, NavigationDebugger* navigationDebugger) override;
    void step(double deltaTime) override;
protected:
    virtual void internalInit() = 0;
    virtual void internalStep(double deltaTime) = 0;

    Hal* hal;
    SharedMemory *shared;
    VisualDebugger *visualDebugger;
    BodyStateMachineControl* control;
    Config* config;
    NavigationDebugger* navigationDebugger;

    BodyInfo bodyInfo;
    BrainInfo brainInfo;

private:
    std::string expectedStep();
};


#endif //PROY_GRADO_BODYSTATEBASE_H
