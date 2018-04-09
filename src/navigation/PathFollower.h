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

class PathFollower {

public:
    PathFollower(Config* config, World* world);
    void setPath(Path path);
    NavigationCommand update(std::vector<Marker> markers, double altitude, double deltaTime);
    std::vector<cv::Vec3d> EstimatedPositions;
    cv::Vec3d EstimatedPosition;
    cv::Vec3d PredictedPosition;
    cv::Vec3d ProjectedPredictedPosition;
    cv::Vec3d TargetOnPath;

    std::vector<cv::Vec3d> EstimatedPoses;
    cv::Vec3d EstimatedPose;

    double runningTime = 0;

    boost::circular_buffer<cv::Vec3d> PositionsHistory;
    boost::circular_buffer<double> DeltaTimeHistory;

    int getTargetId();
    void setTarget(int target);

    static constexpr double TARGET_APROXIMATION_DISTANCE = 2;
    static constexpr double TARGET_REACHED_DISTANCE = 1;
    static constexpr double ALIGNEMENT_ANGLE_THRESOLD = 15;
    static constexpr double DISPLACEMENT_MAX_VELOCITY = 0.15;
    static constexpr double YAW_MAX_VELOCITY = 0.15;

    static constexpr double NEXT_POSITION_MICROSECONDS = 1000000;
private:

    Config* config;
    World* world;
    WorldObject* drone;
    Path path;
    int currentTarget = 0;

    int lastYawSign = 1;
    NavigationCommand lastCommand;
    long timeSinceLastMarkerSeen = 0;

    double distanceToMarker(Marker m);

    Point getAngularDisplacement(cv::Point2i markerCenter);

    void EstimatePosition(const std::vector<Marker> &markers, double altitude);

    double signedMod(double a, double n);

    double angleDifference(double a, double b);

    cv::Vec3d previousEstimation;

    void SmoothEstimation();

    void EstimateNextPosition();

    void ProjectNextPosition();

};

#endif //PROY_GRADO_MARKERFOLLOWER_H
