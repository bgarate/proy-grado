#ifndef PROY_GRADO_MARKERFOLLOWER_H
#define PROY_GRADO_MARKERFOLLOWER_H


#include "../config/Config.h"
#include "../hal/Point.h"
#include "World.h"
#include "MarkerTracker.h"
#include <boost/circular_buffer.hpp>

struct NavigationCommand {
    NavigationCommand():ForwardSpeed(0), LateralSpeed(0), YawSpeed(0), Gaz(0){};
    NavigationCommand(double forwardSpeed, double lateralSpeed,double yawSpeed, double gaz)
            : ForwardSpeed(forwardSpeed), LateralSpeed(lateralSpeed),YawSpeed(yawSpeed), Gaz(gaz){

    }
    double ForwardSpeed;
    double LateralSpeed;
    double YawSpeed;
    double Gaz;
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

    double startAlignementDistance;
    double targetReachedDistance;
    double alignmentAngleThreshold;

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

    void EstimatePosition(const std::vector<Marker> &markers, double altitude);

    void SmoothEstimation();

    void EstimateNextPosition();

    void ProjectNextPosition();

};

#endif //PROY_GRADO_MARKERFOLLOWER_H
