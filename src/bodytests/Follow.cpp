//
// Created by bruno on 18/05/17.
//

#include <zconf.h>
#include <src/logging/Logger.h>
#include <src/tracking/DetectAndTrack.h>
#include <src/tracking/HogDetector.h>
#include <src/tracking/MultiTracker.h>
#include <src/VisualDebugger.h>
#include <src/tracking/Follower.h>
#include <src/tracking/CascadeDetector.h>
#include "BodyTest.h"
#include "../hal/hal.hpp"

class Follow : public BodyTest {


    Hal* hal;
    int currentStep = 0;
    double currentTime = 0;

    bool waitingTakeOff = false;
    bool waitingLanding = false;
    bool tookOff = false;

    DetectionAlgorithm* detector;
    TrackingAlgorithm* tracker;
    DetectAndTrack* detectAndTrack;
    Follower* follower;

    double targetAltitude = 3;
    double altitudeSlowdownRadius = 1;

    bool inmc = false;

    VisualDebugger* visualDebugger;

    void InitBodyTest(Hal *hal, Config* config, VisualDebugger* visualDebugger) override {
        this->hal = hal;

        detector = new CascadeDetector();
        tracker = new MultiTracker(MultiTracker::Algorithm::KCF);
        detectAndTrack =  new DetectAndTrack(detector, tracker);
        follower = new Follower(config);

        std::shared_ptr<cv::Mat> frame = hal->getFrame(Camera::Front);
        config->setFrameSize(cv::Point(frame->size().width,frame->size().height));

        this->visualDebugger = visualDebugger;

    }

    bool BodyTestStep(double deltaTime) override {

        std::shared_ptr<cv::Mat> frame = hal->getFrame(Camera::Front);

        if (hal->getState() == State::Landed && !tookOff) {
            // Despegar
            Logger::logError("Despegar");
            visualDebugger->writeConsole("Despegar");
            hal->takeoff();
            tookOff = true;
            waitingTakeOff = true;
            return true;
        } else if (waitingTakeOff &&
                   (hal->getState() == State::Hovering || hal->getState() == State::Flying)) {
            Logger::logError("Despegado");
            visualDebugger->writeConsole("Despegado");
            // Despegado
            waitingTakeOff = false;
        } else if (waitingLanding && hal->getState() == State::Landed) {
            // Aterrizado
            Logger::logError("Aterrizado");
            visualDebugger->writeConsole("Aterrizado");
            return false;
        } else {

            currentTime += deltaTime;

            if (frame != NULL) {

                std::vector<Track> objects = detectAndTrack->update(frame);
                visualDebugger->setTracks(objects);

                if(objects.empty())
                    follower->stopFollowing();
                else if(!follower->isFollowing())
                    follower->setFollowee(objects.front().getNumber());

                FollowCommand followCommand = follower->follow(objects, hal->getAltitude(),deltaTime);

                if(follower->isFollowing())
                    visualDebugger->setFollowCommand(followCommand);

                visualDebugger->drawHorizon(follower->getHorizon());

                double currentAltitude = hal->getAltitude();
                double deltaAltitude = targetAltitude - currentAltitude;
                double gaz = std::max(-1.0,std::min(1.0,(deltaAltitude/altitudeSlowdownRadius)))* 100;

                hal->move(0,(int)(followCommand.outputDisplacement.Pitch()*100),
                          (int)(followCommand.outputRotation.Yaw() * 100),(int)gaz);

            }
        }

        return true;

    }

    void FinishBodyTest() override {
        if(hal->getState() != State::Landed)
            hal->land();
    }

};