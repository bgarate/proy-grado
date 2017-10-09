//
// Created by bruno on 24/09/17.
//

#ifndef PROY_GRADO_CONFIGDEFAULTS_H
#define PROY_GRADO_CONFIGDEFAULTS_H

#include <src/navigation/World.h>

class Config;

class ConfigDefaults {
public:
    static void SetDefaults(Config *config);

    static World getWorld(Config *config);
};


#endif //PROY_GRADO_CONFIGDEFAULTS_H
