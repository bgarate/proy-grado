//
// Created by bruno on 12/08/17.
//

#include <src/tracking/Follower.h>
#include <src/logging/Logger.h>
#include <src/ConfigKeys.h>
#include "MarkerFollower.h"

MarkerFollower::MarkerFollower(Config *config, World *world) {
    this->config = config;
    this->world = world;
    drone = world->getDrones()[0];
}

void MarkerFollower::setPath(std::vector<int> path) {
    this->path = std::vector<int>(path);
    currentTarget = 0;
}

NavigationCommand MarkerFollower::update(std::vector<Marker> markers, double altitude, double deltaTime) {

    EstimatePosition(markers, altitude);

    WorldObject* target = world->getMarker(path[currentTarget]);

    cv::Vec3d targetVector = target->getPosition() - EstimatedPosition;
    double distanceToTarget = cv::norm(targetVector);
    double alignmentAngle = angleDifference(target->getRotation()[2],EstimatedPose[2]);

    bool alignToMarker = true;

    if(distanceToTarget <= TARGET_REACHED_DISTANCE &&
            std::abs(alignmentAngle) > ALIGNEMENT_ANGLE_THRESOLD) {
        currentTarget = (currentTarget + 1) % path.size();
        return NavigationCommand();
    } else if(distanceToTarget > TARGET_APROXIMATION_DISTANCE) {
        alignToMarker = false;
    }

    double targetAngle;

    if(alignToMarker)
        targetAngle = alignmentAngle;
    else
        targetAngle = toDegrees(std::atan(targetVector[0]/targetVector[1]));

    double forwardSpeed = std::min(distanceToTarget / TARGET_APROXIMATION_DISTANCE,1.0) * DISPLACEMENT_MAX_VELOCITY;
    double yawSpeed = std::max(std::min(targetAngle / ALIGNEMENT_ANGLE_THRESOLD,1.0),-1.0) * YAW_MAX_VELOCITY;

    return NavigationCommand(forwardSpeed, yawSpeed);

}

int MarkerFollower::getTargetId() {

    return path[currentTarget];
}

double MarkerFollower::angleDifference(double target, double origin){
    double  diff = target - origin;
    diff = signedMod((diff + 180),360.0) - 180;

    return diff;
}

double MarkerFollower::signedMod(double a, double n) {
    return a - std::floor(a/n) * n;
}

void MarkerFollower::EstimatePosition(const std::vector<Marker> &markers, double altitude) {

    if(markers.size() == 0)
        return;

    EstimatedPositions.clear();
    EstimatedPoses.clear();

    for(const Marker& marker : markers) {

        WorldObject* markerDescription = world->getMarker(marker.Id);

        if(markerDescription == NULL)
            return;

        Point angularDisplacement = getAngularDisplacement(marker.getCenter());

        double effectiveAltitude = altitude - markerDescription->getPosition()[2];

        cv::Vec3d posXyz = marker.getXYZPosition();
        double groundDistance2 = sqrt(posXyz[0]*posXyz[0] + posXyz[1]*posXyz[1]);

        /*double verticalAngle = 90 - config->Get(ConfigKeys::Drone::CameraTilt)() - angularDisplacement.Tilt();
        double groundDistance1 = effectiveAltitude * tan(toRadians(verticalAngle));*/

        //std::cout << "Distance estimation difference:" << groundDistance1 << " " << groundDistance2 << std::endl;

        double groundDistance = groundDistance2;//(groundDistance1 + groundDistance2)/2;

        //double horizontalAngle1 = angularDisplacement.Pan();
        double horizontalAngle2 = toDegrees(atan(posXyz[0] / posXyz[1]));

        // TODO: Revisar
        double horizontalAngle = horizontalAngle2; //horizontalAngle1 / 4 + horizontalAngle2 / 2;

        //std::cout << "Angle estimation difference:" << horizontalAngle1 << " " << horizontalAngle2 << std::endl;

        double estimatedMarkerAngle = marker.getEulerAngles()[2];

        double estimatedCameraAngle = markerDescription->getRotation()[2] - estimatedMarkerAngle;


        cv::Vec2d estimatedTranslation =
                cv::Vec2d(sin(toRadians(estimatedCameraAngle)) * groundDistance,
                          cos(toRadians(estimatedCameraAngle)) * groundDistance);

        EstimatedPoses.push_back(cv::Vec3d(0, 0, estimatedCameraAngle));

        cv::Vec3d estimatedPosition = markerDescription->getPosition();
        estimatedPosition[2] = altitude;
        estimatedPosition[0] -= estimatedTranslation[0];
        estimatedPosition[1] -= estimatedTranslation[1];

        EstimatedPositions.push_back(estimatedPosition);

        //std::cout << "Camera angle: " << estimatedCameraAngle << std::endl;;

    }

    EstimatedPosition = cv::Vec3d(0, 0, 0);
    EstimatedPose = cv::Vec3d(0, 0, 0);

    double sines = 0;
    double cosines = 0;

    for(int i = 0; i < EstimatedPositions.size(); i++) {
        EstimatedPosition += EstimatedPositions[i];
        sines += sin(toRadians(EstimatedPoses[i][2]));
        cosines += cos(toRadians(EstimatedPoses[i][2]));
    }

    EstimatedPosition[0] /= EstimatedPositions.size();
    EstimatedPosition[1] /= EstimatedPositions.size();
    EstimatedPosition[2] /= EstimatedPositions.size();

    // Ver https://en.wikipedia.org/wiki/Mean_of_circular_quantities
    EstimatedPose[2] = toDegrees(atan2(sines, cosines));
    //std::cout << EstimatedPose[2] << std::endl;

}

Point MarkerFollower::getAngularDisplacement(cv::Point2i markerCenter) {

    cv::Size frameSize = config->Get(ConfigKeys::Drone::FrameSize);

    cv::Point frameCenter = cv::Point(frameSize.width/2, frameSize.height/2);

    double displacementY = markerCenter.y - frameCenter.y;
    double displacementX = markerCenter.x - frameCenter.x;


    double tgPan = displacementX/(frameSize.width/2)*std::tan(toRadians(config->Get(ConfigKeys::Drone::FOV)/2));
    double tgTilt = displacementY/(frameSize.height/2)*std::tan(toRadians(config->Get(ConfigKeys::Drone::VerticalFOV)/2));

    return Point(toDegrees(std::atan(tgPan)),toDegrees(std::atan(tgTilt)),0);

}


double MarkerFollower::toDegrees(double rad) {
    return rad / M_PI * 180;
}

double MarkerFollower::toRadians(double deg) {
    return deg / 180 * M_PI;
}

double MarkerFollower::distanceToMarker(Marker m) {
    return cv::norm(m.Translation);
}
