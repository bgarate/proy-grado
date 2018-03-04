//
// Created by bruno on 04/03/18.
//

#ifndef PROY_GRADO_BODYSTATEMACHINECONTROL_H
#define PROY_GRADO_BODYSTATEMACHINECONTROL_H

#include "string"

class ITransition {
public:
    virtual void Transition(std::string nextStep);
};

class BodyStateMachineControl {
public:
    BodyStateMachineControl(ITransition* stateMachine):stateMachine(stateMachine) {

    }

    void Transition(std::string nextStep) {
        stateMachine->Transition(nextStep);
    }
private:
    ITransition* stateMachine;
};

#endif //PROY_GRADO_BODYSTATEMACHINECONTROL_H
