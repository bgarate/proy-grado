//
// Created by bruno on 12/08/17.
//

#ifndef PROY_GRADO_MARKERFOLLOWER_H
#define PROY_GRADO_MARKERFOLLOWER_H


#include "../config/Config.h"
#include "../hal/Point.h"
#include "World.h"
#include "MarkerTracker.h"

struct NavigationCommand {
    NavigationCommand():ForwardSpeed(0), YawSpeed(0) {};
    NavigationCommand(double forwardSpeed, double yawSpeed)
            : ForwardSpeed(forwardSpeed), YawSpeed(yawSpeed){

    }
    double ForwardSpeed;
    double YawSpeed;
};

class MarkerFollower {

public:
    MarkerFollower(Config* config, World* world);
    void setPath(std::vector<int> path);
    NavigationCommand update(std::vector<Marker> markers, double altitude, double deltaTime);
    std::vector<cv::Vec3d> EstimatedPositions;
    cv::Vec3d EstimatedPosition;
    std::vector<cv::Vec3d> EstimatedPoses;
    cv::Vec3d EstimatedPose;

    int getTargetId();

    static constexpr double TARGET_APROXIMATION_DISTANCE = 2.0;
    static constexpr double TARGET_REACHED_DISTANCE = 0.75;
    static constexpr double ALIGNEMENT_ANGLE_THRESOLD = 15;
    static constexpr double DISPLACEMENT_MAX_VELOCITY = 0.15;
    static constexpr double YAW_MAX_VELOCITY = 0.5;
private:

    Config* config;
    World* world;
    WorldObject* drone;
    std::vector<int> path;
    int currentTarget = 0;

    double distanceToMarker(Marker m);

    double toDegrees(double rad);

    double toRadians(double deg);

    Point getAngularDisplacement(cv::Point2i markerCenter);

    void EstimatePosition(const std::vector<Marker> &markers, double altitude);

    double signedMod(double a, double n);

    double angleDifference(double a, double b);
};


#endif //PROY_GRADO_MARKERFOLLOWER_H
