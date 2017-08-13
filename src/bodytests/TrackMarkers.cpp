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

    double targetAltitude = 1.5;
    double altitudeSlowdownRadius = 1;

    void InitBodyTest(Hal *hal, Config* config, VisualDebugger* visualDebugger) override {
        this->hal = hal;
        this->visualDebugger = visualDebugger;
        tracker = new MarkerTracker(config);

        world.addMarker(cv::Vec3d(0,0,0),cv::Vec3d(0,0,0), 0);
        world.addMarker(cv::Vec3d(0,2.65,0),cv::Vec3d(0,0,0), 5);
        world.addMarker(cv::Vec3d(0,2.65+2.05,0),cv::Vec3d(0,0,0), 1);
        world.addMarker(cv::Vec3d(0,2.65+2.05+1.56,0),cv::Vec3d(0,0,90), 8);

        world.addMarker(cv::Vec3d(1.74,2.65+2.05+1.56,0),cv::Vec3d(0,0,90), 4);
        world.addMarker(cv::Vec3d(1.74+1.47,2.65+2.05+1.56,0),cv::Vec3d(0,0,180), 11);

        world.addMarker(cv::Vec3d(1.74+1.47,1.40+1.40+1.9,0),cv::Vec3d(0,0,180), 9);
        world.addMarker(cv::Vec3d(1.74+1.47,1.40+1.40,0),cv::Vec3d(0,0,180), 10);
        world.addMarker(cv::Vec3d(1.74+1.47,1.40,0),cv::Vec3d(0,0,180), 6);
        world.addMarker(cv::Vec3d(1.74+1.47,0,0),cv::Vec3d(0,0,-90), 7);

        world.addMarker(cv::Vec3d(1.67,0,0),cv::Vec3d(0,0,-90), 3);
        world.addMarker(cv::Vec3d(1.67,2.75,0),cv::Vec3d(0,0,0), 2);

        world.addDrone(cv::Vec3d(5,0,0),cv::Vec3d(0,0,0), config->getId());

        drone = world.getDrones()[0];

        follower = new MarkerFollower(config, &world);

        navigationDebugger = new NavigationDebugger(config, &world);
        navigationDebugger->Init();

        std::vector<int> path = {0,5,1,8,4,11,9,10,6,7,3};

        follower->setPath(path);

        //navigationDebuggerThread = std::thread(&NavigationDebugger::Run, navigationDebugger);
    }

    bool tookOff = false;
    bool waitingTakeOff = false;

    bool BodyTestStep(double deltaTime) override {

        std::shared_ptr<cv::Mat> frame = hal->getFrame(Camera::Front);

        //DESPEGAR
        if (hal->getState() == State::Landed && !tookOff) {
            // Despegar
            Logger::logError("Despegar");
            visualDebugger->writeConsole("Despegar");
            hal->takeoff();
            tookOff = true;
            waitingTakeOff = true;
            return true;

            //DESPEGADO
        } else if (waitingTakeOff &&
                   (hal->getState() == State::Hovering || hal->getState() == State::Flying)) {
            Logger::logError("Despegado");
            visualDebugger->writeConsole("Despegado");
            // Despegado
            waitingTakeOff = false;

        //MOVERSE
        } else {

            if (frame != NULL && !frame->empty()) {
                tracker->Update(frame, deltaTime);
                NavigationCommand command = follower->update(tracker->Markers, hal->getAltitude(), deltaTime);

                drone->setPosition(follower->EstimatedPosition);
                drone->setRotation(follower->EstimatedPose);

                visualDebugger->ShowMarkers(tracker->Markers);
                visualDebugger->setNavigationCommand(command);
                navigationDebugger->Run(command,
                                        follower->getTargetId(), follower->EstimatedPositions,
                                        follower->EstimatedPoses);

                /*if(tracker->Markers.size()>0) {
                    world.getDrones()[0]->setInversePose(
                            cv::Vec3d(tracker->Markers[0].Translation),cv::Vec3d(tracker->Markers[0].Rotation));
                }*/

                double currentAltitude = hal->getAltitude();
                double deltaAltitude = targetAltitude - currentAltitude;
                double gaz = std::max(-1.0, std::min(1.0, (deltaAltitude / altitudeSlowdownRadius)));

                hal->move(0, (int) (command.ForwardSpeed * 100), (int) (command.YawSpeed * 100), (int) (gaz * 100));
            }

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