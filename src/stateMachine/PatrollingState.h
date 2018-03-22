//
// Created by bruno on 04/03/18.
//

#ifndef PROY_GRADO_PATROLLINGSTATE_H
#define PROY_GRADO_PATROLLINGSTATE_H

#include <src/communication/SharedMemory.h>
#include <src/debugging/VisualDebugger.h>
#include <src/navigation/NavigationDebugger.h>
#include "BodyState.h"
#include "BodyStateMachineControl.h"
#include "StepName.h"

class PatrollingState: public BodyState {
public:

    std::string getName() override {
        return StepName::PATROLLING;
    }

    void init(Config* config, Hal* hal, SharedMemory* shared, BodyStateMachineControl* control,  VisualDebugger* visualDebugger, NavigationDebugger* navigationDebugger) override {
        this->hal = hal;
        this->shared = shared;
        this->visualDebugger = visualDebugger;
    }

    void prepare() override {

    }

    void step(double deltaTime) override {

        BodyInfo info = shared->getBodyInfo();
        NavigationCommand command = info.FollowPathCommand;

        double currentAltitude = hal->getAltitude();
        double deltaAltitude = targetAltitude - currentAltitude;
        double gaz = std::max(-1.0, std::min(1.0, (deltaAltitude / altitudeSlowdownRadius)));

        visualDebugger->setNavigationCommand(command);
        hal->move((int)(command.LateralSpeed * 100), (int) (command.ForwardSpeed * 100), (int) (command.YawSpeed * 100), (int) (gaz * 100));

        info.ExecutedCommand = command;
        shared->setBodyInfo(info);
    }

    void leave() override {

    }

    void cleanup() override {

    }
private:
    Hal* hal;
    SharedMemory *shared;
    VisualDebugger *visualDebugger;

    double targetAltitude = 2;
    double altitudeSlowdownRadius = 1;
};

#endif //PROY_GRADO_PATROLLINGSTATE_H
