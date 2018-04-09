//
// Created by bruno on 30/03/18.
//

#ifndef PROY_GRADO_COMMANDGENERATOR_H
#define PROY_GRADO_COMMANDGENERATOR_H


#include <src/utils/Helpers.h>
#include "PathFollower.h"

class CommandGenerator {

public:

    static constexpr double TARGET_APROXIMATION_DISTANCE = 2;
    static constexpr double ALIGNEMENT_ANGLE_THRESOLD = 15;
    static constexpr double DISPLACEMENT_MAX_VELOCITY = 0.15;
    static constexpr double YAW_MAX_VELOCITY = 0.30;

    CommandGenerator(cv::Vec3d position, double rotation): Position(position), Rotation(rotation){

    }

    NavigationCommand getCommand(cv::Vec3d targetPosition, double targetRotation){

        cv::Vec3d targetVector3d = targetPosition - Position;
        cv::Vec2d targetVector2d = cv::Vec2d(targetVector3d[0],targetVector3d[1]);

        cv::Vec2d normalizedTarget = cv::normalize(targetVector2d);
        cv::Vec2d targetVector = Helpers::rotate(normalizedTarget,Helpers::toRadians(Rotation));

        double distanceToPathPoint = cv::norm(targetVector2d);

        double alignmentAngle = Helpers::angleDifference(targetRotation,Rotation);

        double speed = std::min(distanceToPathPoint / TARGET_APROXIMATION_DISTANCE,1.0) * DISPLACEMENT_MAX_VELOCITY;

        double forwardSpeed = targetVector[1] * speed;
        double lateralSpped = targetVector[0] * speed;
        double yawSpeed = std::max(std::min(alignmentAngle / ALIGNEMENT_ANGLE_THRESOLD,1.0),-1.0) * YAW_MAX_VELOCITY;

        return NavigationCommand(forwardSpeed,lateralSpped, yawSpeed);
    }

private:
    cv::Vec3d Position;
    double Rotation;

};


#endif //PROY_GRADO_COMMANDGENERATOR_H
