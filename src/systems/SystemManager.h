//
// Created by bruno on 04/03/18.
//

#ifndef PROY_GRADO_SYSTEMMANAGER_H
#define PROY_GRADO_SYSTEMMANAGER_H


#include <src/communication/SharedMemory.h>
#include "System.h"
#include <typeinfo>

class SystemManager {

public:
    void RegisterSystem(ISystem *system) {
        systems.push_back(system);
    }

    void Init(Config* config, Hal* hal, SharedMemory* shared, VisualDebugger* visualDebugger, NavigationDebugger* navigationDebugger) {
        for (ISystem *s: systems) {
            s->Init(config, hal, shared, visualDebugger, navigationDebugger);
        }
    }

    void Step(double deltaTime) {
        for (ISystem *s: systems) {
            if(s->Enabled)
                s->Update(deltaTime);
        }
    }

    void Cleanup() {
        for (ISystem *s: systems) {
            s->Cleanup();
        }
    }

    template <class T>
    T* Get(){
        for (ISystem *s: systems) {
            if (typeid(*s) == typeid(T))
                return (T*)s;
        }

        throw new std::runtime_error("System not found");
    }

    template <class T>
    void Enable() {
        Get<T>()->Enabled = true;
    }

    template <class T>
    void Disable() {
        Get<T>()->Enabled = false;
    }

private:
    std::vector<ISystem *> systems;


};


#endif //PROY_GRADO_SYSTEMMANAGER_H
