//
// Created by bruno on 30/03/18.
//

#include "BodyStateBase.h"
#include "StepName.h"


bool BodyStateBase::shouldTransition() {
    std::string expected = expectedStep();
    return !expected.empty() && getName() != expected;
}

void BodyStateBase::doTransition() {
    control->Transition(expectedStep());
}

std::string BodyStateBase::expectedStep() {
    switch (shared->getBrainInfo().currentTask){
        case BrainInfo::PATROLING:
            return StepName::PATROLLING;
        case BrainInfo::FOLLOWING:
            return StepName::FOLLOWING;
        case BrainInfo::ALERT:
            return StepName::ALERT;
        case BrainInfo::BACKFROMPAD:
            return StepName::GOING_TO_PATH;
        case BrainInfo::GOINGTOPAD:
            return StepName::GOING_TO_PAD;
        case BrainInfo::SHUTDOWN:
            return StepName::SHUTDOWN;
        case BrainInfo::INNACTIVE:
        case BrainInfo::CHARGED:
        case BrainInfo::CHARGING:
            return "";
    }

    throw std::runtime_error("Unknown step");
}

void BodyStateBase::init(Config *config, Hal *hal, SharedMemory *shared, BodyStateMachineControl *control,
                         VisualDebugger *visualDebugger, NavigationDebugger *navigationDebugger) {

    this->hal = hal;
    this->shared = shared;
    this->visualDebugger = visualDebugger;
    this->control = control;
    this->config = config;
    this->navigationDebugger = navigationDebugger;

    internalInit();
}

void BodyStateBase::step(double deltaTime) {
    brainInfo = shared->getBrainInfo();
    bodyInfo = shared->getBodyInfo();

    if(shouldTransition()){
        doTransition();
        return;
    }

    visualDebugger->setSubStatus(getName());
    internalStep(deltaTime);

    shared->setBodyInfo(bodyInfo);
}
