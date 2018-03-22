//
// Created by bruno on 04/03/18.
//

#ifndef PROY_GRADO_MARKERTRACKERSYSTEM_H
#define PROY_GRADO_MARKERTRACKERSYSTEM_H


#include <src/navigation/MarkerTracker.h>
#include <src/navigation/NavigationDebugger.h>
#include <src/hal/hal.hpp>
#include <src/debugging/VisualDebugger.h>
#include "System.h"

class MarkerTrackerSystem: public ISystem {
public:

    void Init(Config* config, Hal* hal, SharedMemory* shared, VisualDebugger* visualDebugger, NavigationDebugger* navigationDebugger) override {
        tracker = new MarkerTracker(config);

        world = config->GetWorld();
        this->hal = hal;
        this->navigationDebugger = navigationDebugger;
        this->visualDebugger = visualDebugger;

        drone = world.getDrones()[0];

        hal->setCameraTilt(Camera::Middle);

    }

    void Update(double deltaTime) override {

        std::shared_ptr<cv::Mat> frame = hal->getFrame(Camera::Front);

        if (frame == NULL || !frame->empty())
            return;

        BodyInfo info = shared->getBodyInfo();

        tracker->Update(frame, deltaTime);
        follower->setTarget(info.CurrentTargetId);
        NavigationCommand command = follower->update(tracker->Markers, hal->getAltitude(), deltaTime);

        drone->setPosition(follower->EstimatedPosition);
        drone->setRotation(follower->EstimatedPose);

        info.CurrentPosition = follower->EstimatedPosition;
        info.PredictedFuturePosition = follower->PredictedPosition;
        info.ProjectedPositionOnPath = follower->ProjectedPredictedPosition;
        info.TargetOnPath = follower->FollowTarget;
        info.CurrentPose = follower->EstimatedPose;
        info.FollowPathCommand = command;
        info.CurrentTargetId = follower->getTargetId();

        shared->setBodyInfo(info);

        visualDebugger->ShowMarkers(tracker->Markers);
        navigationDebugger->setVisibleMarkers(tracker->Markers);
        navigationDebugger->SetEstimations(follower->EstimatedPositions, follower->EstimatedPoses);
        navigationDebugger->SetPositionHistory(follower->PositionsHistory);

    }

    void Cleanup() override {

    }

private:
    Config* config;
    SharedMemory* shared;

    MarkerTracker* tracker;
    NavigationDebugger* navigationDebugger;
    VisualDebugger* visualDebugger;
    MarkerFollower* follower;

    World world;
    WorldObject* drone;
    Hal* hal;
};


#endif //PROY_GRADO_MARKERTRACKERSYSTEM_H
