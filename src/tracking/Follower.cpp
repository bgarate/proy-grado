//
// Created by bruno on 28/05/17.
//

#include <cmath>
#include "Follower.h"

FollowCommand::FollowCommand() {

}


FollowCommand Follower::follow(std::vector<Track> tracks, double altitude, double deltaTime) {

    FollowCommand followCommand;

    if (!isFollowing())
        return followCommand;

    std::vector<Track>::iterator iterator = std::find_if(tracks.begin(), tracks.end(),
        [this](Track t){return t.getNumber() == this->followee;});

    if (iterator == tracks.end()) {
        followee = NOT_FOLLOWING;
        return followCommand;
    }

    Track track = *iterator;

    ++iterator;
    if (iterator != tracks.end())
        throw new std::runtime_error("More than one track with same id");

    Point angularDisplacement = getAngularDisplacement(track);

    double verticalAngle = 90 - config->getCameraTilt() - config->getVerticalFov()/2 + angularDisplacement.Tilt();
    double horizontalAngle = angularDisplacement.Pan();

    double distance = altitude * std::tan(toRadians(verticalAngle));
    double horizontalDistance = altitude * std::tan(toRadians(horizontalAngle));

    followCommand.linearDisplacement = Point(horizontalDistance, distance, altitude);
    followCommand.angularDisplacement = angularDisplacement;
    followCommand.outputDisplacement = getDisplacement(distance, deltaTime);
    followCommand.outputRotation = getRotation(horizontalAngle, deltaTime);

    return followCommand;

}

Point Follower::getDisplacement(double distance, double deltaTime) {

    Point displacement = Point();

    double dif = distance - TARGET_DISTANCE;

    if(dif <= 0)
        return displacement;

    // TODO: La velocidad del drone se obtiene Norte-Este-Abajo. Hay que emplear
    // esto y la orientación del drone respecto a una brújula y el acelerómetro para determinar
    // su velocidad respecto a ejes locales.

    displacement.Pitch(std::min((dif / TARGET_DISTANCE_SLOW_DOWN_RADIUS),1.0) *
                               DISPLACEMENT_MAX_VELOCITIY);

    return displacement;
};

Point Follower::getRotation(double horizontalAngle, double deltaTime) {

    Point rotation = Point();

    if(std::abs(horizontalAngle) <= EPSILON_ANGULAR_DIFFERENCE)
        return rotation;

    // TODO: Falta emplear la velocidad angular actual y deltaTime

    rotation.Yaw(std::min((horizontalAngle / TARGET_YAW_SLOW_DOWN_RADIUS),1.0) *
                       YAW_MAX_VELOCITY);

    return rotation;
};

Point Follower::getAngularDisplacement(Track track) {

    cv::Size frameSize = config->getFrameSize();

    cv::Point frameCenter = cv::Point(frameSize.width/2, frameSize.height/2);
    cv::Point trackCenter = cv::Point((int)(track.getRect().x + track.getRect().width / 2),
                                      (int)(track.getRect().y + track.getRect().height / 2));

    double displacementX = trackCenter.x - frameCenter.x;

    // DisplacementY se mide desde la base del track, asumiendo que es el punto de
    // contacto con el piso
    double displacementY = (track.getRect().y + track.getRect().height) - frameCenter.x;

    double tgPan = displacementX/(frameSize.width/2)*std::tan(toRadians(config->getFov()));
    double tgTilt = displacementY/(frameSize.height/2)*std::tan(toRadians(config->getVerticalFov()));

    return Point(toDegrees(std::atan(tgPan)),toDegrees(std::atan(tgTilt)),0);

}

void Follower::setFollowee(int followee) {
    this->followee = followee;
}

bool Follower::isFollowing() {
    return followee != NOT_FOLLOWING;
}

cv::Point Follower::getFolloweeVelocity() {
    return followeeVelocity;
}

Follower::Follower(Config* config) {
    this->config = config;
}

double Follower::toDegrees(double rad) {
    return rad / M_PI * 180;
}

double Follower::toRadians(double deg) {
    return deg / 180 * M_PI;
}
