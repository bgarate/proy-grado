//
// Created by bruno on 28/05/17.
//

#ifndef PROY_GRADO_FOLLOWER_H
#define PROY_GRADO_FOLLOWER_H


#include <src/hal/hal.hpp>
#include "Track.h"
#include "../hal/Point.h"

class FollowCommand {
public:
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

    cv::Point getFolloweeVelocity();
private:
    static const int NOT_FOLLOWING = -1;

    static constexpr const double TARGET_DISTANCE = 5;
    static constexpr const double TARGET_DISTANCE_SLOW_DOWN_RADIUS = 2;
    static constexpr const double DISPLACEMENT_MAX_VELOCITIY = 1;

    static constexpr const double EPSILON_ANGULAR_DIFFERENCE = 10;
    static constexpr const double TARGET_YAW_SLOW_DOWN_RADIUS = 20;
    static constexpr const double YAW_MAX_VELOCITY = 1;


    cv::Point followeeVelocity;
    int followee;

    Config* config = NULL;

    Point getAngularDisplacement(Track track);

    inline double toDegrees(double rad);
    inline double toRadians(double deg);

    Point getDisplacement(double distance, double deltaTime);

    Point getRotation(double horizontalAngle, double deltaTime);
};


#endif //PROY_GRADO_FOLLOWER_H
