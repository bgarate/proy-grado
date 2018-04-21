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

        hal->setCameraTilt(Camera::Bottom);

        if(landing) {
            if(hal->getState() == State::Landed){
                bodyInfo.landedInPad = true;
            }
        } if(landingCommand.state == LandingState::Inactive ||
           std::abs(landingCommand.roll)+std::abs(landingCommand.pitch)+std::abs(landingCommand.yaw)<0.0001) {

            visualDebugger->setSubStatus("going-to-pad locating");

            CommandGenerator generator(bodyInfo.CurrentPosition, bodyInfo.CurrentPose[2], config);
            int padId = brainInfo.currentPadId;
            WorldObject* pad = world.getPad(padId);

            cv::Vec3d targetPosition = pad->getPosition();
            targetPosition[2] = bodyInfo.CurrentPosition[2];

            NavigationCommand command = generator.getCommand(targetPosition, pad->getRotation()[2]);

            hal->move((int)(command.LateralSpeed * 100), (int) (command.ForwardSpeed * 100), (int) (command.YawSpeed * 100), (int) (command.Gaz * 100));

        } else if(!landingCommand.land) {
            visualDebugger->setSubStatus("going-to-pad tracking");
            hal->move((int)(landingCommand.roll*100),(int)(landingCommand.pitch*100), (int)(-landingCommand.yaw * 100),(int)(landingCommand.gaz * 100));
        } else {
            hal->land();
        }

    }

};

#endif //PROY_GRADO_GOTOPAD_H
