//
// Created by bruno on 30/03/18.
//

#include "FollowingState.h"
#include "StepName.h"

void FollowingState::internalInit() {

}

void FollowingState::internalStep(double deltaTime) {
    if(bodyInfo.intruderDetected) {

        NavigationCommand command = bodyInfo.FollowDetectionCommand;

        double currentAltitude = hal->getAltitude();
        double deltaAltitude = targetAltitude - currentAltitude;
        double gaz = std::max(-1.0, std::min(1.0, (deltaAltitude / altitudeSlowdownRadius)));

        hal->move((int)(command.LateralSpeed * 100), (int) (command.ForwardSpeed * 100), (int) (command.YawSpeed * 100), (int) (gaz * 100));

        bodyInfo.ExecutedCommand = command;

    }
}

std::string FollowingState::getName() {
    return StepName::FOLLOWING;
}

void FollowingState::prepare() {

}

void FollowingState::leave() {

}

void FollowingState::cleanup() {

}
