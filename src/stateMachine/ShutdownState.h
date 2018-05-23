//
// Created by bruno on 31/03/18.
//

#ifndef PROY_GRADO_SHUTDOWNSTATE_H
#define PROY_GRADO_SHUTDOWNSTATE_H

#include "BodyStateBase.h"
#include "StepName.h"

class ShutdownState: public BodyStateBase {
public:
    std::string getName() override {
        return StepName::SHUTDOWN;
    }

    void prepare() override {

    }

    void leave() override {

    }

    void cleanup() override {

    }

protected:
    void internalInit() override {

    }

    void internalStep(double deltaTime) override {
        control->DoShutdown();
    }
};

#endif //PROY_GRADO_SHUTDOWNSTATE_H
