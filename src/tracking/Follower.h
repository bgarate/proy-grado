//
// Created by bruno on 28/05/17.
//

#ifndef PROY_GRADO_FOLLOWER_H
#define PROY_GRADO_FOLLOWER_H


#include "Track.h"
#include "../hal/Point.h"
#include "src/config/Config.h"

class FollowCommand {
public:
    int followee;
    Point linearDisplacement;
    Point angularDisplacement;
    Point outputDisplacement;
    Point outputRotation;
    FollowCommand();
};

class Follower {
public:
    Follower(Config* config);
    FollowCommand follow(std::vector<Track> tracks, double altitude, double deltaTime);
    void setFollowee(int followee);
    bool isFollowing();
    void stopFollowing();
    cv::Point getFolloweeVelocity();
    static const int NOT_FOLLOWING = -1;

    static constexpr const double YAW_MAX_VELOCITY = 1;
    static constexpr const double DISPLACEMENT_MAX_VELOCITIY = 0.3;

    FollowCommand getCommand(double altitude, double deltaTime, const cv::Point &trackPoint);

    int getHorizon();

private:


    static constexpr const double TARGET_DISTANCE = 5;
    static constexpr const double TARGET_DISTANCE_SLOW_DOWN_RADIUS = 2;

    static constexpr const double TARGET_YAW_SLOW_DOWN_RADIUS = 20;


    cv::Point followeeVelocity;
    int followee = NOT_FOLLOWING;

    Config* config = NULL;

    Point getAngularDisplacement(cv::Point2i trackProint);

    inline double toDegrees(double rad);
    inline double toRadians(double deg);

    Point getDisplacement(double distance, double deltaTime);

    Point getRotation(double horizontalAngle, double deltaTime);

    double distance(cv::Point a, cv::Point b);
};


#endif //PROY_GRADO_FOLLOWER_H
