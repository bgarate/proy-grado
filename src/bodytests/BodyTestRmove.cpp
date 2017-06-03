//
// Created by bruno on 18/05/17.
//

#include <zconf.h>
#include <src/logging/Logger.h>
#include <src/tracking/DetectAndTrack.h>
#include <src/tracking/HogDetector.h>
#include <src/tracking/MultiTracker.h>
#include <src/VisualDebugger.h>
#include "BodyTest.h"
#include "../hal/hal.hpp"

class BodyTestRmove : public BodyTest {

    typedef struct DirectionTime {
        DirectionTime():x(0),y(0),z(0),s(0){};
        DirectionTime(double x, double y, double z):x(x), y(y),z(0), s(1){}
        DirectionTime(double x, double y, double z, double s):x(x), y(y),z(0), s(s){}
        double x;
        double y;
        double z;
        double s;
    } DirectionTime;


    Hal* hal;

    bool waitingTakeOff = false;
    bool waitingLanding = false;
    bool tookOff = false;
    bool rmovemode = false;

    DetectionAlgorithm* detector;
    TrackingAlgorithm* tracker;
    DetectAndTrack* detectAndTrack;

    bool inmc = false;

    VisualDebugger* visualDebugger;

    void InitBodyTest(Hal *hal, VisualDebugger* visualDebugger) override {
        this->hal = hal;

        detector = new HogDetector();
        tracker = new MultiTracker(MultiTracker::Algorithm::KCF);
        detectAndTrack =  new DetectAndTrack(detector, tracker);

        this->visualDebugger = visualDebugger;

        Logger::logInfo("Bateria: %u") << hal->bateryLevel();
    }

    bool BodyTestStep(double deltaTime) override {

        std::shared_ptr<cv::Mat> frame = hal->getFrame(Camera::Front);

        if (frame != NULL) {
            visualDebugger->setFrame(frame);
        }

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
        } else if (rmovemode && !hal->isRmoving() && !waitingLanding) {
            // Aterrizar
            Logger::logError("Aterrizar");
            visualDebugger->writeConsole("Aterrizar");
            hal->land();
            waitingLanding = true;
            return true;

        } else if (!rmovemode && !waitingLanding && !waitingTakeOff) {


            hal->rmove(0,0,-3,0);
            rmovemode=true;
            Logger::logError("Do rmove");

            if (frame != NULL) {

                //test track begin
                //update the tracking result
                std::vector<Track> objects = detectAndTrack->update(frame);
                visualDebugger->setTracks(objects);

            }
        }

        return true;

    }

    void FinishBodyTest() override {
        if(hal->getState() != State::Landed)
            hal->land();
    }

};