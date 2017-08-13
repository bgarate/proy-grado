//
// Created by bruno on 18/05/17.
//

#include <zconf.h>
#include <opencv2/core/matx.hpp>
#include <src/navigation/MarkerTracker.h>
#include <src/navigation/World.h>
#include <src/navigation/NavigationDebugger.h>
#include <src/navigation/MarkerFollower.h>
#include "../logging/Logger.h"
#include "../tracking/DetectAndTrack.h"
#include "../tracking/HogDetector.h"
#include "../tracking/MultiTracker.h"
#include "../VisualDebugger.h"
#include "BodyTest.h"
#include "../hal/hal.hpp"

class TrackMarkers : public BodyTest {

    Hal* hal;
    int currentStep = 0;
    double currentTime = 0;
    World world;

    VisualDebugger* visualDebugger;
    MarkerTracker* tracker;
    std::thread navigationDebuggerThread;
    NavigationDebugger* navigationDebugger;
    MarkerFollower* follower;

    WorldObject* drone;

    double targetAltitude = 3;
    double altitudeSlowdownRadius = 1;

    void InitBodyTest(Hal *hal, Config* config, VisualDebugger* visualDebugger) override {
        this->hal = hal;
        this->visualDebugger = visualDebugger;
        tracker = new MarkerTracker(config);

        world.addMarker(cv::Vec3d(0,1,0),cv::Vec3d(0,0,0), 0);
        world.addMarker(cv::Vec3d(0,1.55,0),cv::Vec3d(0,0,-90), 16);
        world.addMarker(cv::Vec3d(-0.32,1.55,0),cv::Vec3d(0,0, 180), 17);
        world.addMarker(cv::Vec3d(-0.32,1,0),cv::Vec3d(0,0, 90), 13);

        world.addDrone(cv::Vec3d(5,0,0),cv::Vec3d(0,0,0), config->getId());

        drone = world.getDrones()[0];

        follower = new MarkerFollower(config, &world);

        navigationDebugger = new NavigationDebugger(config, &world);
        navigationDebugger->Init();

        std::vector<int> path = {0,16,17,13};

        follower->setPath(path);

        //navigationDebuggerThread = std::thread(&NavigationDebugger::Run, navigationDebugger);
    }

    bool BodyTestStep(double deltaTime) override {

        std::shared_ptr<cv::Mat> frame = hal->getFrame(Camera::Front);
        if(frame != NULL && !frame->empty()) {
            tracker->Update(frame, deltaTime);
            NavigationCommand command = follower->update(tracker->Markers, hal->getAltitude(), deltaTime);

            drone->setPosition(follower->EstimatedPosition);
            drone->setRotation(follower->EstimatedPose);

            visualDebugger->ShowMarkers(tracker->Markers);
            visualDebugger->setNavigationCommand(command);
            navigationDebugger->Run(command,
                                    follower->getTargetId(),follower->EstimatedPositions, follower->EstimatedPoses);

            /*if(tracker->Markers.size()>0) {
                world.getDrones()[0]->setInversePose(
                        cv::Vec3d(tracker->Markers[0].Translation),cv::Vec3d(tracker->Markers[0].Rotation));
            }*/

            double currentAltitude = hal->getAltitude();
            double deltaAltitude = targetAltitude - currentAltitude;
            double gaz = std::max(-1.0, std::min(1.0, (deltaAltitude / altitudeSlowdownRadius)));

            hal->move(0, (int)(command.ForwardSpeed * 100),(int)(command.YawSpeed * 100), (int)(gaz * 100));
        }

        return true;

    }

    void FinishBodyTest() override {
        if(hal->getState() != State::Landed)
            hal->land();

        navigationDebugger->Shutdown();
        //navigationDebuggerThread.join();

        delete tracker;
        delete navigationDebugger;
        delete follower;
    }

};