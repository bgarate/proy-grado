//
// Created by bruno on 04/03/18.
//

#ifndef PROY_GRADO_SYSTEM_H
#define PROY_GRADO_SYSTEM_H

#include <src/config/Config.h>
#include <src/communication/SharedMemory.h>
#include "src/hal/hal.hpp"
#include "src/debugging/VisualDebugger.h"
#include "src/navigation/NavigationDebugger.h"

class ISystem {

public:

    virtual void Init(Config* config, Hal* hal, SharedMemory* shared, VisualDebugger* visualDebugger, NavigationDebugger* navigationDebugger) = 0;
    virtual void Update(long deltaTime) = 0;
    virtual void Cleanup() = 0;
    virtual std::string GetName() = 0;
    bool Enabled;

};

#endif //PROY_GRADO_SYSTEM_H
