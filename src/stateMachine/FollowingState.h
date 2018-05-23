//
// Created by bruno on 30/03/18.
//

#ifndef PROY_GRADO_FOLLOWINGSTATE_H
#define PROY_GRADO_FOLLOWINGSTATE_H


#include "BodyStateBase.h"

class FollowingState : public BodyStateBase {
public:
    void internalInit() override;

    void internalStep(double deltaTime) override;

    std::string getName() override;

    void prepare() override;

    void leave() override;

    void cleanup() override;

private:
    double targetAltitude = 2;
    double altitudeSlowdownRadius = 1;

};


#endif //PROY_GRADO_FOLLOWINGSTATE_H
