//
// Created by bruno on 31/03/18.
//

#ifndef PROY_GRADO_ALERTSTATE_H
#define PROY_GRADO_ALERTSTATE_H

#include <src/navigation/CommandGenerator.h>
#include "BodyStateBase.h"
#include "StepName.h"

class AlertState : public BodyStateBase {

public:
    std::string getName() override {
        return StepName::ALERT;
    }

    void prepare() override {
        control->getSystemManager()->Disable<FollowerSystem>();
        control->getSystemManager()->Disable<PadLandingSystem>();
        control->getSystemManager()->Enable<MarkerTrackerSystem>();
        control->getSystemManager()->Enable<BatterySystem>();
    }

    void leave() override {

    }

    void cleanup() override {

    }
private:
    double targetAltitude = 2;
    double altitudeSlowdownRadius = 1;
protected:
    void internalInit() override {

    }

    void internalStep(double deltaTime) override {
        CommandGenerator generator(bodyInfo.CurrentPosition, bodyInfo.CurrentPose[2], config);

        cv::Vec2d target(brainInfo.alertedPosition[0] - bodyInfo.CurrentPosition[0],
                         brainInfo.alertedPosition[1] - bodyInfo.CurrentPosition[1]);

        NavigationCommand command = generator.getCommand(bodyInfo.CurrentPosition, Helpers::toAngle(target));

        hal->move((int)(command.LateralSpeed * 100), (int) (command.ForwardSpeed * 100), (int) (command.YawSpeed * 100), (int) (command.Gaz * 100));
    }

};

#endif //PROY_GRADO_ALERTSTATE_H
