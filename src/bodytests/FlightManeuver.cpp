//
// Created by bruno on 18/05/17.
//

#include <zconf.h>
#include "../logging/Logger.h"
#include "../tracking/DetectAndTrack.h"
#include "../tracking/HogDetector.h"
#include "../tracking/MultiTracker.h"
#include "../VisualDebugger.h"
#include "../tracking/Follower.h"
#include "../tracking/CascadeDetector.h"
#include "BodyTest.h"
#include "../hal/hal.hpp"

class FlightManeuver : public BodyTest {

    typedef struct DirectionTime {
        DirectionTime():x(0),y(0),z(0),s(0){};
        DirectionTime(double x, double y, double z):x(x), y(y),z(z), s(1){}
        DirectionTime(double x, double y, double z, double s):x(x), y(y),z(z), s(s){}
        double x;
        double y;
        double z;
        double s;
    } DirectionTime;


    Hal* hal;
    int currentStep = 0;
    double currentTime = 0;
    /*std::vector<DirectionTime> sequence = {
            DirectionTime(-0.25,0,0,2),
            DirectionTime(0,0,0,2),
            DirectionTime(0,-0.25,0,2),
            DirectionTime(0,0,0,2),
            DirectionTime(0.25,0,0,2),
            DirectionTime(0,0,0,2),
            DirectionTime(0,0.25,0,2)};*/

    std::vector<DirectionTime> sequence = {DirectionTime(0,0,0.25,1),DirectionTime(0,0,0,100)};

    bool waitingTakeOff = false;
    bool waitingLanding = false;
    bool tookOff = false;

    DetectionAlgorithm* detector;
    TrackingAlgorithm* tracker;
    DetectAndTrack* detectAndTrack;
    Follower* follower;

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

        Logger::logInfo("Bateria: %u") << hal->bateryLevel();
    }

    bool BodyTestStep(double deltaTime) override {

        std::shared_ptr<cv::Mat> frame = hal->getFrame(Camera::Front);

        /*if (frame != NULL) {
            visualDebugger->setFrame(frame);
        }*/

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
        } else if (currentStep >= sequence.size() && !waitingLanding) {
            // Aterrizar
            Logger::logError("Aterrizar");
            visualDebugger->writeConsole("Aterrizar");
            hal->land();
            waitingLanding = true;
            return true;
        } else if (!waitingLanding && !waitingTakeOff) {


            DirectionTime directionTime = sequence[currentStep];

            hal->move((int) (directionTime.y * 100), (int) (directionTime.x * 100),
                      0, (int) (directionTime.z * 100));

            if (currentTime >= directionTime.s * 1000000) {
                currentTime = 0;
                currentStep++;
                Logger::logInfo("Next element in flight maneuver sequence");
                visualDebugger->writeConsole("Next element in flight maneuver sequence");
            } else {
                currentTime += deltaTime;
            }

            if (frame != NULL) {

                //test track begin
                //update the tracking result
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

            }
        }

        return true;

    }

    void FinishBodyTest() override {
        if(hal->getState() != State::Landed)
            hal->land();
    }

};