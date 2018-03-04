//
// Created by bruno on 04/03/18.
//

#ifndef PROY_GRADO_SYSTEMMANAGER_H
#define PROY_GRADO_SYSTEMMANAGER_H


#include <src/communication/SharedMemory.h>
#include "System.h"

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

private:
    std::vector<ISystem *> systems;


};


#endif //PROY_GRADO_SYSTEMMANAGER_H
