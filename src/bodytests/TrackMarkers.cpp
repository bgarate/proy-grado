//
// Created by bruno on 18/05/17.
//

#include <zconf.h>
#include <opencv2/core/matx.hpp>
#include <src/navigation/MarkerTracker.h>
#include <src/navigation/World.h>
#include <src/navigation/NavigationDebugger.h>
#include <src/navigation/MarkerFollower.h>
#include <src/config/ConfigKeys.h>
#include "../logging/Logger.h"
#include "../tracking/DetectAndTrack.h"
#include "../tracking/HogDetector.h"
#include "../tracking/MultiTracker.h"
#include "src/debugging/VisualDebugger.h"
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

    Path path;

    void InitBodyTest(Hal *hal, Config* config, VisualDebugger* visualDebugger) override {
        this->hal = hal;
        this->visualDebugger = visualDebugger;
        tracker = new MarkerTracker(config);

        world = config->GetWorld();

        drone = world.getDrones()[0];

        follower = new MarkerFollower(config, &world);

        navigationDebugger = new NavigationDebugger(config, &world);
        navigationDebugger->Init();


        this->path = config->GetPath();

        follower->setPath(path);
        //navigationDebuggerThread = std::thread(&NavigationDebugger::Run, navigationDebugger);
    }

    bool tookOff = false;
    bool waitingTakeOff = false;

    bool BodyTestStep(double deltaTime) override {

        std::shared_ptr<cv::Mat> frame = hal->getFrame(Camera::Front);
        hal->setCameraTilt(Camera::Middle);

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
                navigationDebugger->setVisibleMarkers(tracker->Markers);
                navigationDebugger->Run(command, follower->getTargetId(), follower->EstimatedPositions,
                                        follower->EstimatedPoses, path,
                                        follower->PositionsHistory, follower->PredictedPosition,
                                        follower->ProjectedPredictedPosition, follower->FollowTarget);

                double currentAltitude = hal->getAltitude();
                double deltaAltitude = targetAltitude - currentAltitude;
                double gaz = std::max(-1.0, std::min(1.0, (deltaAltitude / altitudeSlowdownRadius)));

                hal->move((int)(command.LateralSpeed * 100), (int) (command.ForwardSpeed * 100), (int) (command.YawSpeed * 100), (int) (gaz * 100));
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