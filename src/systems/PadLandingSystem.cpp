//
// Created by bruno on 30/03/18.
//

#include "PadLandingSystem.h"

void PadLandingSystem::Cleanup() {

}

void PadLandingSystem::internalInit() {

    this->markTrack = new MarkerTrack();
    this->markerLand = new MarkerLand();

}

void PadLandingSystem::internalUpdate(double deltaTime) {

    std::shared_ptr<cv::Mat> frame = hal->getFrame(Camera::Front);

    std::vector<cv::Point> squarePoints = markTrack->Track(frame);
    visualDebugger->setSquareTracks(squarePoints);

    cv::Point frameSize(frame->size().width,frame->size().height);

    bodyInfo.PadLandingCommand = markerLand->land(squarePoints, frameSize,hal->getAltitude());

}
