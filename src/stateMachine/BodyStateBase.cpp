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
        case BrainInfo::INNACTIVE:
            return "";
        case BrainInfo::PATROLING:
            return StepName::PATROLLING;
        case BrainInfo::FOLLOWING:
            return StepName::FOLLOWING;
        case BrainInfo::ALERT:
            return "";
        case BrainInfo::CHARGED:
        case BrainInfo::CHARGING:
            return "";
        case BrainInfo::BACKFROMPAD:
            return "";
        case BrainInfo::GOINGTOPAD:
            return StepName::GOING_TO_PAD;
    }
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

    internalStep(deltaTime);

    shared->setBodyInfo(bodyInfo);
}
