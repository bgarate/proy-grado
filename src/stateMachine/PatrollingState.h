//
// Created by bruno on 04/03/18.
//

#ifndef PROY_GRADO_PATROLLINGSTATE_H
#define PROY_GRADO_PATROLLINGSTATE_H

#include <src/communication/SharedMemory.h>
#include <src/debugging/VisualDebugger.h>
#include <src/navigation/NavigationDebugger.h>
#include <src/systems/FollowerSystem.h>
#include <src/systems/PadLandingSystem.h>
#include "IBodyState.h"
#include "BodyStateMachineControl.h"
#include "StepName.h"
#include "BodyStateBase.h"

class PatrollingState: public BodyStateBase {
public:

    std::string getName() override {
        return StepName::PATROLLING;
    }

    void prepare() override {
        control->getSystemManager()->Disable<FollowerSystem>();
        control->getSystemManager()->Disable<PadLandingSystem>();

        control->getSystemManager()->Enable<MarkerTrackerSystem>();
    }

    void internalInit() override {

    }

    void internalStep(double deltaTime) override {

        NavigationCommand command = bodyInfo.FollowPathCommand;

        double currentAltitude = hal->getAltitude();
        double deltaAltitude = targetAltitude - currentAltitude;
        double gaz = std::max(-1.0, std::min(1.0, (deltaAltitude / altitudeSlowdownRadius)));

        visualDebugger->setNavigationCommand(command);
        hal->move((int)(command.LateralSpeed * 100), (int) (command.ForwardSpeed * 100), (int) (command.YawSpeed * 100), (int) (gaz * 100));

        bodyInfo.ExecutedCommand = command;

    }

    void leave() override {

    }

    void cleanup() override {

    }

private:
    double targetAltitude = 2;
    double altitudeSlowdownRadius = 1;
};

#endif //PROY_GRADO_PATROLLINGSTATE_H
