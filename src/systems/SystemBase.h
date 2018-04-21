//
// Created by bruno on 30/03/18.
//

#ifndef PROY_GRADO_SYSTEMBASE_H
#define PROY_GRADO_SYSTEMBASE_H


#include <src/navigation/NavigationDebugger.h>
#include <src/debugging/VisualDebugger.h>
#include "System.h"

class SystemBase : public ISystem {
public:
    void Init(Config* config, Hal* hal, SharedMemory* shared, VisualDebugger* visualDebugger, NavigationDebugger* navigationDebugger) override;
    void Update(long deltaTime) override;

protected:
    virtual void internalInit() = 0;
    virtual void internalUpdate(double deltaTime) = 0;

    Hal* hal;
    SharedMemory *shared;
    VisualDebugger *visualDebugger;
    Config* config;
    NavigationDebugger* navigationDebugger;

    BodyInfo bodyInfo;
    BrainInfo brainInfo;
};


#endif //PROY_GRADO_SYSTEMBASE_H
