//
// Created by bruno on 28/05/17.
//

#ifndef PROY_GRADO_FOLLOWER_H
#define PROY_GRADO_FOLLOWER_H


#include <src/hal/hal.hpp>
#include "Track.h"

class FollowCommand {
public:
    Point displacement;
    Point rotation;
    FollowCommand();
    FollowCommand(Point displacement, Point rotation);
};

class Follower {
public:
    FollowCommand follow(std::vector<Track> tracks);
    void setFollowee(int followee);
    bool isFollowing();

    void setSize(cv::Size size);
    cv::Point getFolloweeVelocity();
private:
    static const int NOT_FOLLOWING = -1;

    static const int SLOW_DOWN_RADIUS_X = 10;
    static const int SLOW_DOWN_RADIUS_Y = 20;

    cv::Point followeeVelocity;
    int followee;
    cv::Size size;
};


#endif //PROY_GRADO_FOLLOWER_H
