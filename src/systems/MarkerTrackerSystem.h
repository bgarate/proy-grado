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
#include "SystemBase.h"

class MarkerTrackerSystem: public SystemBase {
public:

    void internalInit() override {
        tracker = new MarkerTracker(config);

        world = config->GetWorld();

        drone = world.getDrones()[0];

        hal->setCameraTilt(Camera::Middle);

    }

    void internalUpdate(double deltaTime) override {

        std::shared_ptr<cv::Mat> frame = hal->getFrame(Camera::Front);

        if (frame == NULL || !frame->empty())
            return;

        tracker->Update(frame, deltaTime);
        follower->setTarget(bodyInfo.CurrentTargetId);
        NavigationCommand command = follower->update(tracker->Markers, hal->getAltitude(), deltaTime);

        drone->setPosition(follower->EstimatedPosition);
        drone->setRotation(follower->EstimatedPose);

        bodyInfo.CurrentPosition = follower->EstimatedPosition;
        bodyInfo.PredictedFuturePosition = follower->PredictedPosition;
        bodyInfo.ProjectedPositionOnPath = follower->ProjectedPredictedPosition;
        bodyInfo.TargetOnPath = follower->TargetOnPath;
        bodyInfo.CurrentPose = follower->EstimatedPose;
        bodyInfo.FollowPathCommand = command;
        bodyInfo.CurrentTargetId = follower->getTargetId();

        visualDebugger->ShowMarkers(tracker->Markers);
        navigationDebugger->setVisibleMarkers(tracker->Markers);
        navigationDebugger->SetEstimations(follower->EstimatedPositions, follower->EstimatedPoses);
        navigationDebugger->SetPositionHistory(follower->PositionsHistory);

    }

    void Cleanup() override {

    }

private:
    MarkerTracker* tracker;
    PathFollower* follower;

    World world;
    WorldObject* drone;
};


#endif //PROY_GRADO_MARKERTRACKERSYSTEM_H
