//
// Created by bruno on 31/03/18.
//

#ifndef PROY_GRADO_GOTOPATH_H
#define PROY_GRADO_GOTOPATH_H


#include <src/navigation/CommandGenerator.h>
#include "BodyStateBase.h"
#include "StepName.h"

class GoToPath : public BodyStateBase {
public:
    std::string getName() override {
        return StepName::GOING_TO_PATH;
    }

    void prepare() override {
        takingOff = false;

        control->getSystemManager()->Disable<FollowerSystem>();
        control->getSystemManager()->Disable<PadLandingSystem>();

        control->getSystemManager()->Enable<MarkerTrackerSystem>();
    }

    void leave() override {

    }

    void cleanup() override {

    }
private:
    bool takingOff = false;
    double targetAltitude = 2;
    double altitudeSlowdownRadius = 1;
    bool landing = false;

    static const constexpr double DISTANTE_PATH_REACHED = 2;
protected:
    void internalInit() override {

    }

    void internalStep(double deltaTime) override {
        if(!takingOff){
            hal->takeoff();
            takingOff = true;
            return;
        }

        State state = hal->getState();

        if(state != State::Hovering && state != State::Flying)
            return;

        CommandGenerator generator(bodyInfo.CurrentPosition, bodyInfo.CurrentPose[2]);

        // TODO: No toma en cuenta el target mas cercano en todos los segmentos de path
        NavigationCommand command = generator.getCommand(bodyInfo.TargetOnPath, bodyInfo.CurrentPose[2]);

        double currentAltitude = hal->getAltitude();
        double deltaAltitude = targetAltitude - currentAltitude;
        double gaz = std::max(-1.0, std::min(1.0, (deltaAltitude / altitudeSlowdownRadius)));

        hal->move((int)(command.LateralSpeed * 100), (int) (command.ForwardSpeed * 100), (int) (command.YawSpeed * 100), (int) (gaz * 100));


        cv::Vec2d d(bodyInfo.ProjectedPositionOnPath[0] - bodyInfo.CurrentPosition[0],
                    bodyInfo.ProjectedPositionOnPath[1] - bodyInfo.CurrentPosition[1]);

        bodyInfo.inPath = cv::norm(d) <= 2;

    }
};


#endif //PROY_GRADO_GOTOPATH_H
