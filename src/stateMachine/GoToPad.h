//
// Created by bruno on 30/03/18.
//

#ifndef PROY_GRADO_GOTOPAD_H
#define PROY_GRADO_GOTOPAD_H

#include <src/stateMachine/BodyStateBase.h>
#include <src/stateMachine/StepName.h>
#include <src/navigation/CommandGenerator.h>

class GoToPad : public BodyStateBase {
public:
    std::string getName() override {
        return StepName::GOING_TO_PAD;
    }

    void prepare() override {

    }

    void leave() override {

    }

    void cleanup() override {

    }

protected:
    void internalInit() override {
        world = config->GetWorld();
    }

    void internalStep(double deltaTime) override {

        CommandGenerator generator(bodyInfo.CurrentPosition, bodyInfo.CurrentPose[2]);
        int padId = brainInfo.currentPadId;
        WorldObject* pad = world.getPad(padId);
        NavigationCommand command = generator.getCommand(pad->getPosition(), pad->getRotation()[2]);

    }
private:
    World world;
};

#endif //PROY_GRADO_GOTOPAD_H
