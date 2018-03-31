//
// Created by bruno on 04/03/18.
//

#ifndef PROY_GRADO_BODYSTATEMACHINECONTROL_H
#define PROY_GRADO_BODYSTATEMACHINECONTROL_H

#include <src/systems/SystemManager.h>
#include "string"

class ITransition {
public:
    virtual void Transition(std::string nextStep) = 0;
    virtual void DoShutdown() = 0;
};

class BodyStateMachineControl {
public:
    BodyStateMachineControl(ITransition* stateMachine, SystemManager* systemManager):
            stateMachine(stateMachine), systemManager(systemManager) {

    }

    void Transition(std::string nextStep) {
        stateMachine->Transition(nextStep);
    }

    void DoShutdown(){
        stateMachine->DoShutdown();
    }

    SystemManager* getSystemManager(){
        return systemManager;
    };
private:
    ITransition* stateMachine;
    SystemManager* systemManager;
};

#endif //PROY_GRADO_BODYSTATEMACHINECONTROL_H
