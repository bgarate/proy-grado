//
// Created by bruno on 30/03/18.
//

#ifndef PROY_GRADO_COMMANDGENERATOR_H
#define PROY_GRADO_COMMANDGENERATOR_H


#include <src/utils/Helpers.h>
#include "PathFollower.h"

class CommandGenerator {

public:

    double TargetAproximationDistance;
    double AlignmentAngleThreshold;
    double DisplacementMaxVelocity;
    double YawMaxVelocity;
    double AltitudeSlowDownRadius;

    CommandGenerator(cv::Vec3d position, double rotation, Config* config): Position(position), Rotation(rotation) {
        TargetAproximationDistance = config->Get(ConfigKeys::Body::TargetSlowdownRadius);
        AlignmentAngleThreshold = config->Get(ConfigKeys::Body::AlignmentAngleThreshold);
        DisplacementMaxVelocity = config->Get(ConfigKeys::Body::DisplacementMaxVelocity);
        YawMaxVelocity = config->Get(ConfigKeys::Body::YawMaxVelocity);
        AltitudeSlowDownRadius = config->Get(ConfigKeys::Body::AltitudeSlowDownRadius);
    }

    NavigationCommand getCommand(cv::Vec3d targetPosition, double targetRotation){

        cv::Vec3d targetVector3d = targetPosition - Position;
        cv::Vec2d targetVector2d = cv::Vec2d(targetVector3d[0],targetVector3d[1]);

        cv::Vec2d normalizedTarget = cv::normalize(targetVector2d);
        cv::Vec2d targetVector = Helpers::rotate(normalizedTarget,Helpers::toRadians(Rotation));

        double distanceToPathPoint = cv::norm(targetVector2d);

        double alignmentAngle = Helpers::angleDifference(targetRotation,Rotation);

        double speed = std::min(distanceToPathPoint / TargetAproximationDistance,1.0) * DisplacementMaxVelocity;

        double forwardSpeed = targetVector[1] * speed;
        double lateralSpped = targetVector[0] * speed;
        double yawSpeed = std::max(std::min(alignmentAngle / AlignmentAngleThreshold,1.0),-1.0) * YawMaxVelocity;

        double deltaAltitude = targetVector[2] - Position[2];
        double gaz = std::max(-1.0, std::min(1.0, (deltaAltitude / AltitudeSlowDownRadius)));

        return NavigationCommand(forwardSpeed,lateralSpped, yawSpeed, gaz);
    }

private:
    cv::Vec3d Position;
    double Rotation;

};


#endif //PROY_GRADO_COMMANDGENERATOR_H
