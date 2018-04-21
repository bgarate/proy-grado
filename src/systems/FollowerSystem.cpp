//
// Created by bruno on 30/03/18.
//

#include <src/tracking/CascadeDetector.h>
#include <src/utils/Helpers.h>
#include <src/config/ConfigKeys.h>
#include "FollowerSystem.h"

void FollowerSystem::Cleanup() {

}

void FollowerSystem::internalInit() {

    detector = new CascadeDetector(config->Get(ConfigKeys::Body::CascadeDetector));
    tracker = new MultiTracker(MultiTracker::Algorithm::KCF);
    detectAndTrack =  new DetectAndTrack(detector, tracker);
    follower = new Follower(config);

}

void FollowerSystem::internalUpdate(double deltaTime) {

    std::shared_ptr<cv::Mat> frame = hal->getFrame(Camera::Front);

    if(frame == NULL)
        return;

    std::vector<Track> objects = detectAndTrack->update(frame);
    visualDebugger->setTracks(objects);

    if (objects.empty())
        follower->stopFollowing();
    else if (!follower->isFollowing())
        follower->setFollowee(objects.front().getNumber());

    FollowCommand command = follower->follow(objects, hal->getAltitude(), deltaTime);

    visualDebugger->drawHorizon(follower->getHorizon());

    bodyInfo.FollowDetectionCommand = NavigationCommand(command.outputDisplacement.Pitch(), 0, command.outputRotation.Yaw(), hal->getAltitude());
    bodyInfo.intruderDetected = follower->isFollowing();

    cv::Vec2d displacement = Helpers::rotate(cv::Vec2d(1,0),Helpers::toRadians(bodyInfo.CurrentPose[2])) * command.linearDisplacement.Norm();
    bodyInfo.intruderPosition =  bodyInfo.CurrentPosition + cv::Vec3d(displacement[0], displacement[1],0);

}
