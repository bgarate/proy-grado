//
// Created by bruno on 04/03/18.
//

#ifndef PROY_GRADO_TAKINGOFFSTATE_H
#define PROY_GRADO_TAKINGOFFSTATE_H


#include <src/debugging/VisualDebugger.h>
#include <src/communication/SharedMemory.h>
#include <src/navigation/NavigationDebugger.h>
#include "BodyState.h"
#include "StepName.h"

class TakingOffState: public BodyState {
public:

    std::string getName() override {
        return StepName::TAKING_OFF;
    }

    void init(Config* config, Hal* hal, SharedMemory* shared, BodyStateMachineControl* control,  VisualDebugger* visualDebugger, NavigationDebugger* navigationDebugger) override {
        this->hal = hal;
        this->shared = shared;
        this->visualDebugger = visualDebugger;
        this->control = control;
    }

    void prepare() override {

    }

    void step(double deltaTime) override {
        if(hal->getState() == State::Landed && !takingOff)
            hal->takeoff();

        takingOff = true;
        visualDebugger->writeConsole("Despegado");

        if(hal->getState() == State::Hovering || hal->getState() == State::Flying)
            control->Transition(StepName::PATROLLING);
    }

    void leave() override {

    }

    void cleanup() override {

    }
private:
    Hal* hal;
    SharedMemory *shared;
    VisualDebugger *visualDebugger;
    bool takingOff = false;
    BodyStateMachineControl* control;
};


#endif //PROY_GRADO_TAKINGOFFSTATE_H
