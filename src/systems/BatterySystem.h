//
// Created by bruno on 31/03/18.
//

#ifndef PROY_GRADO_BATTERYSYSTEM_H
#define PROY_GRADO_BATTERYSYSTEM_H

#include "SystemBase.h"

class BatterySystem : public SystemBase {

public:
    void Cleanup() override {

    }

protected:
    void internalInit() override {

    }

    void internalUpdate(double deltaTime) override {

        bodyInfo.batteryLevel = hal->bateryLevel();

    }

};

#endif //PROY_GRADO_BATTERYSYSTEM_H
