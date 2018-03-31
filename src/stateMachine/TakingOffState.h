//
// Created by bruno on 04/03/18.
//

#ifndef PROY_GRADO_TAKINGOFFSTATE_H
#define PROY_GRADO_TAKINGOFFSTATE_H


#include <src/debugging/VisualDebugger.h>
#include <src/communication/SharedMemory.h>
#include <src/navigation/NavigationDebugger.h>
#include <src/systems/PadLandingSystem.h>
#include <src/systems/FollowerSystem.h>
#include "IBodyState.h"
#include "StepName.h"
#include "BodyStateBase.h"

class TakingOffState: public BodyStateBase {
public:

    std::string getName() override {
        return StepName::TAKING_OFF;
    }

    void prepare() override {
        control->getSystemManager()->Disable<FollowerSystem>();
        control->getSystemManager()->Disable<PadLandingSystem>();
        control->getSystemManager()->Disable<MarkerTrackerSystem>();
    }

    void internalInit() override {

    }

    void internalStep(double deltaTime) override {

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
    bool takingOff = false;
};


#endif //PROY_GRADO_TAKINGOFFSTATE_H
