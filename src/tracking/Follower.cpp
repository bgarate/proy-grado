//
// Created by bruno on 28/05/17.
//

#include "Follower.h"

FollowCommand::FollowCommand() {

}

FollowCommand::FollowCommand(Point displacement, Point rotation) :
        displacement(displacement), rotation(rotation) {

}

FollowCommand Follower::follow(std::vector<Track> tracks) {
/*
    FollowCommand followCommand;
    
    if(!isFollowing())
        return followCommand;

    std::vector<Track>::iterator iterator = std::find(tracks.begin(), tracks.end(), followee);

    if(iterator == tracks.end()){
        followee = NOT_FOLLOWING;
        return followCommand;
    }

    Track track = *iterator;

    ++iterator;
    if(iterator != tracks.end())
        throw new std::runtime_error("More than one track with same id");

    cv::Point frameCenter = cv::Point(size.width/2, size.height/2);
    cv::Point trackCenter = cv::Point((int)(track.getRect().x + track.getRect().width / 2),
                                      (int)(track.getRect().y + track.getRect().height / 2));

    cv::Point displacement = trackCenter - frameCenter;

*/

}

void Follower::setFollowee(int followee) {

}

void Follower::setSize(cv::Size size) {
    this->size = size;
}

bool Follower::isFollowing() {
    return false;
}

cv::Point Follower::getFolloweeVelocity() {
    return followeeVelocity;
}
