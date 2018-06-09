//
// Created by bruno on 30/03/18.
//

#include <src/systems/FollowerSystem.h>
#include <src/systems/MarkerTrackerSystem.h>
#include <src/systems/PadLandingSystem.h>
#include <src/systems/BatterySystem.h>
#include "FollowingState.h"
#include "StepName.h"

void FollowingState::internalInit() {

}

void FollowingState::internalStep(double deltaTime) {

    hal->setCameraTilt(Camera::Front);

    NavigationCommand command;

    if(bodyInfo.intruderDetected)
        command = bodyInfo.FollowDetectionCommand;

    visualDebugger->setNavigationCommand(command);
    hal->move((int)(command.LateralSpeed * 100), (int) (command.ForwardSpeed * 100), (int) (command.YawSpeed * 100), (int) (command.Gaz * 100));

    bodyInfo.ExecutedCommand = command;


}

std::string FollowingState::getName() {
    return StepName::FOLLOWING;
}

void FollowingState::prepare() {
    control->getSystemManager()->Disable<PadLandingSystem>();
    control->getSystemManager()->Enable<FollowerSystem>();
    control->getSystemManager()->Disable<MarkerTrackerSystem>();
    control->getSystemManager()->Enable<BatterySystem>();
}

void FollowingState::leave() {

}

void FollowingState::cleanup() {

}
