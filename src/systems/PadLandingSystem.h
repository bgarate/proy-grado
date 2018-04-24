//
// Created by bruno on 30/03/18.
//

#ifndef PROY_GRADO_PADLANDINGSYSTEM_H
#define PROY_GRADO_PADLANDINGSYSTEM_H


#include <src/landtracking/MarkerLand.h>
#include <src/landtracking/MarkerTrack.h>
#include "SystemBase.h"

class PadLandingSystem : public SystemBase {

    void Cleanup() override;

    void internalInit() override;

    void internalUpdate(double deltaTime) override;
    std::string GetName() override;

private:
    MarkerTrack* markTrack;
    MarkerLand* markerLand;

};


#endif //PROY_GRADO_PADLANDINGSYSTEM_H
