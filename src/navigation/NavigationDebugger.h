//
// Created by bruno on 09/08/17.
//

#ifndef PROY_GRADO_NAVIGATIONDEBUGGER_H
#define PROY_GRADO_NAVIGATIONDEBUGGER_H


#include <src/Config.h>
#include <pangolin/pangolin.h>
#include "World.h"


class NavigationDebugger {
public:
    NavigationDebugger(Config* config, World* world);
    void Run();
private:
    Config* config;
    World *world;
};


#endif //PROY_GRADO_NAVIGATIONDEBUGGER_H
