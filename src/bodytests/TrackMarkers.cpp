//
// Created by bruno on 18/05/17.
//

#include <zconf.h>
#include <opencv2/core/matx.hpp>
#include <src/navigation/MarkerTracker.h>
#include <src/navigation/World.h>
#include <src/navigation/NavigationDebugger.h>
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

    WorldObject* drone;

    void InitBodyTest(Hal *hal, Config* config, VisualDebugger* visualDebugger) override {
        this->hal = hal;
        this->visualDebugger = visualDebugger;
        tracker = new MarkerTracker(config);

        world.addMarker(cv::Vec3d(0,0,0),cv::Vec3d(0,0,0), 2);
        world.addMarker(cv::Vec3d(0,10,0),cv::Vec3d(0,0,0), 1);
        world.addMarker(cv::Vec3d(5,0,0),cv::Vec3d(0,0,0), 0);

        world.addDrone(cv::Vec3d(5,0,0),cv::Vec3d(0,0,0), 0);

        drone = world.getDrones()[0];

        navigationDebugger = new NavigationDebugger(config, &world);

        navigationDebuggerThread = std::thread(&NavigationDebugger::Run, navigationDebugger);
    }

    bool BodyTestStep(double deltaTime) override {

        std::shared_ptr<cv::Mat> frame = hal->getFrame(Camera::Front);
        if(frame != NULL && !frame->empty()) {
            tracker->Update(frame, deltaTime);
            visualDebugger->ShowMarkers(tracker->Markers);
            if(tracker->Markers.size()>0)
                world.getDrones()[0]->setPosition(
                        cv::Vec3d(tracker->Markers[0].EstimatedCameraPose));
        }

        return true;

    }

    void FinishBodyTest() override {
        if(hal->getState() != State::Landed)
            hal->land();

        navigationDebuggerThread.join();

        delete tracker;
        delete navigationDebugger;
    }

};