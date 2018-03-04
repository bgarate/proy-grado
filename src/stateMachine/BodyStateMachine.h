//
// Created by bruno on 04/03/18.
//

#ifndef PROY_GRADO_BODYSTATEMACHINE_H
#define PROY_GRADO_BODYSTATEMACHINE_H


#include "BodyState.h"
#include "BodyStateMachineControl.h"

class BodyStateMachine: public ITransition {
public:

    void RegisterState(BodyState* state){
        states[state->getName()] = state;
    }

    void Init(std::string firstStep,Config* config, Hal* hal, SharedMemory* shared, VisualDebugger* visualDebugger, NavigationDebugger* navigationDebugger) {

        BodyStateMachineControl control(this);

        for(const std::pair<std::string,BodyState*>& pair: states){
            pair.second->init(config, hal, shared, control,visualDebugger, navigationDebugger);
        }

        currentState = states[firstStep];
        currentState->prepare();
    }

    void Step(double deltaTime) {
        currentState->step(deltaTime);
    }

    virtual void Transition(std::string nextStep){

        Logger::logInfo("Transitioning form %s to %s") << currentState->getName() << nextStep;

        currentState->leave();
        currentState = states[nextStep];
        currentState->prepare();

    }

    void Cleanup() {
        currentState->leave();

        for(const std::pair<std::string,BodyState*>& pair: states){
            pair.second->cleanup();
        }

    }

private:
    std::map<std::string, BodyState*> states;
    BodyState* currentState = NULL;
};


#endif //PROY_GRADO_BODYSTATEMACHINE_H
