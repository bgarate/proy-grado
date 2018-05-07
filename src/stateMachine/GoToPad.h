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
        control->getSystemManager()->Enable<BatterySystem>();
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

        //Ya aterrizó
        if(landing) {
            if(hal->getState() == State::Landed){
                bodyInfo.landedInPad = true;
            }
        } if(landingCommand.state == LandingState::Inactive || landingCommand.state == LandingState::Lost) {

            visualDebugger->setSubStatus("going-to-pad locating");

            CommandGenerator generator(bodyInfo.CurrentPosition, bodyInfo.CurrentPose[2], config);
            int padId = brainInfo.currentPadId;
            WorldObject* pad = world.getPad(padId);

            cv::Vec3d targetPosition = pad->getPosition();
            targetPosition[2] = bodyInfo.CurrentPosition[2];

            NavigationCommand command = generator.getCommand(targetPosition, pad->getRotation()[2]);

            hal->move((int)(command.LateralSpeed * 100), (int) (command.ForwardSpeed * 100), (int) (command.YawSpeed * 100), (int) (command.Gaz * 100));

        //Se está alineando
        } else if(!landingCommand.land) {
            visualDebugger->setSubStatus("going-to-pad tracking");
            hal->move((int)(landingCommand.roll*100),(int)(landingCommand.pitch*100), (int)(-landingCommand.yaw * 100),(int)(landingCommand.gaz * 100));
        //Aterrizar!
        } else {
            hal->land();
            landing = true;
        }

        if(landingCommand.state == LandingState::Inactive)
            std::cout << "Inactive" << std::endl;
        if(landingCommand.state == LandingState::Centring)
            std::cout << "Centring" << std::endl;
        if(landingCommand.state == LandingState::Lost)
            std::cout << "Lost" << std::endl;
        if(landingCommand.state == LandingState::Landing)
            std::cout << "Landing" << std::endl;
    }

};

#endif //PROY_GRADO_GOTOPAD_H
