//
// Created by bruno on 12/08/17.
//

#ifndef PROY_GRADO_MARKERFOLLOWER_H
#define PROY_GRADO_MARKERFOLLOWER_H


#include "../config/Config.h"
#include "../hal/Point.h"
#include "World.h"
#include "MarkerTracker.h"
#include <boost/circular_buffer.hpp>

struct NavigationCommand {
    NavigationCommand():ForwardSpeed(0), YawSpeed(0) {};
    NavigationCommand(double forwardSpeed, double lateralSpeed,double yawSpeed)
            : ForwardSpeed(forwardSpeed), LateralSpeed(lateralSpeed),YawSpeed(yawSpeed){

    }
    double ForwardSpeed;
    double LateralSpeed;
    double YawSpeed;
};

class MarkerFollower {

public:
    MarkerFollower(Config* config, World* world);
    void setPath(Path path);
    NavigationCommand update(std::vector<Marker> markers, double altitude, double deltaTime);
    std::vector<cv::Vec3d> EstimatedPositions;
    cv::Vec3d EstimatedPosition;
    cv::Vec3d PredictedPosition;
    cv::Vec3d ProjectedPredictedPosition;
    cv::Vec3d FollowTarget;

    std::vector<cv::Vec3d> EstimatedPoses;
    cv::Vec3d EstimatedPose;

    double runningTime = 0;

    boost::circular_buffer<cv::Vec3d> PositionsHistory;
    boost::circular_buffer<double> DeltaTimeHistory;

    int getTargetId();

    static constexpr double TARGET_APROXIMATION_DISTANCE = 0.5;
    static constexpr double TARGET_REACHED_DISTANCE = 0.2;
    static constexpr double ALIGNEMENT_ANGLE_THRESOLD = 15;
    static constexpr double DISPLACEMENT_MAX_VELOCITY = 0.15;
    static constexpr double YAW_MAX_VELOCITY = 0.5;

    static constexpr double NEXT_POSITION_MICROSECONDS = 1000000;
private:

    Config* config;
    World* world;
    WorldObject* drone;
    Path path;
    int currentTarget = 0;

    double distanceToMarker(Marker m);

    double toDegrees(double rad);

    double toRadians(double deg);

    Point getAngularDisplacement(cv::Point2i markerCenter);

    void EstimatePosition(const std::vector<Marker> &markers, double altitude);

    cv::Vec3d previousEstimation;

    void SmoothEstimation();

    void EstimateNextPosition();

    void ProjectNextPosition();

    cv::Vec2d Rotate(cv::Vec2d v, double angle);
};


#endif //PROY_GRADO_MARKERFOLLOWER_H
