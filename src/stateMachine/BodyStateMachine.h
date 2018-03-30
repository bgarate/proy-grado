//
// Created by bruno on 04/03/18.
//

#ifndef PROY_GRADO_BODYSTATEMACHINE_H
#define PROY_GRADO_BODYSTATEMACHINE_H


#include "IBodyState.h"
#include "BodyStateMachineControl.h"

class BodyStateMachine: public ITransition {
public:

    void RegisterState(IBodyState* state){
        states[state->getName()] = state;
    }

    void Init(std::string firstStep,Config* config, Hal* hal, SharedMemory* shared, VisualDebugger* visualDebugger, NavigationDebugger* navigationDebugger) {

        BodyStateMachineControl* control = new BodyStateMachineControl(this);

        for(const std::pair<std::string,IBodyState*>& pair: states){
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

        for(const std::pair<std::string,IBodyState*>& pair: states){
            pair.second->cleanup();
        }

    }

private:
    std::map<std::string, IBodyState*> states;
    IBodyState* currentState = NULL;
};


#endif //PROY_GRADO_BODYSTATEMACHINE_H
