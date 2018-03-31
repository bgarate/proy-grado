//
// Created by bruno on 30/03/18.
//

#ifndef PROY_GRADO_GOTOPAD_H
#define PROY_GRADO_GOTOPAD_H

#include <src/stateMachine/BodyStateBase.h>
#include <src/stateMachine/StepName.h>
#include <src/navigation/CommandGenerator.h>
#include <src/systems/PadLandingSystem.h>

class GoToPad : public BodyStateBase {
public:
    std::string getName() override {
        return StepName::GOING_TO_PAD;
    }

    void prepare() override {
        landing = false;

        control->getSystemManager()->Disable<FollowerSystem>();

        control->getSystemManager()->Enable<PadLandingSystem>();
        control->getSystemManager()->Enable<MarkerTrackerSystem>();
    }

    void leave() override {

    }

    void cleanup() override {

    }
private:
    World world;
    double targetAltitude = 2;
    double altitudeSlowdownRadius = 1;
    bool landing = false;
protected:
    void internalInit() override {
        world = config->GetWorld();
    }

    void internalStep(double deltaTime) override {

        LandMoveCommand landingCommand = bodyInfo.PadLandingCommand;

        if(landing) {
            if(hal->getState() == State::Landed){
                bodyInfo.landedInPad = true;
            }
        } if(landingCommand.state == LandingState::Inactive ||
           std::abs(landingCommand.roll)+std::abs(landingCommand.pitch)+std::abs(landingCommand.yaw)>0.0001) {

            CommandGenerator generator(bodyInfo.CurrentPosition, bodyInfo.CurrentPose[2]);
            int padId = brainInfo.currentPadId;
            WorldObject* pad = world.getPad(padId);
            NavigationCommand command = generator.getCommand(pad->getPosition(), pad->getRotation()[2]);

            double currentAltitude = hal->getAltitude();
            double deltaAltitude = targetAltitude - currentAltitude;
            double gaz = std::max(-1.0, std::min(1.0, (deltaAltitude / altitudeSlowdownRadius)));

            hal->move((int)(command.LateralSpeed * 100), (int) (command.ForwardSpeed * 100), (int) (command.YawSpeed * 100), (int) (gaz * 100));

        } else if(!landingCommand.land) {
            hal->move((int)(landingCommand.roll*100),(int)(landingCommand.pitch*100), (int)(-landingCommand.yaw * 100),(int)(landingCommand.gaz * 100));
        } else {
            hal->land();
        }

    }

};

#endif //PROY_GRADO_GOTOPAD_H
