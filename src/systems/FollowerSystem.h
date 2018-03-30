//
// Created by bruno on 30/03/18.
//

#ifndef PROY_GRADO_FOLLOWERSYSTEM_H
#define PROY_GRADO_FOLLOWERSYSTEM_H


#include <src/tracking/DetectAndTrack.h>
#include "SystemBase.h"

class FollowerSystem : public SystemBase {
    void Cleanup() override;

    void internalInit() override;

    void internalUpdate(double deltaTime) override;
private:
    DetectionAlgorithm* detector;
    TrackingAlgorithm* tracker;
    DetectAndTrack* detectAndTrack;
    Follower* follower;
};


#endif //PROY_GRADO_FOLLOWERSYSTEM_H
