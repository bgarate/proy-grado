//
// Created by bruno on 18/05/17.
//

#include <zconf.h>
#include <src/logging/Logger.h>
#include <src/tracking/DetectAndTrack.h>
#include <src/tracking/HogDetector.h>
#include <src/tracking/MultiTracker.h>
#include <src/debugging/VisualDebugger.h>
#include "BodyTest.h"
#include "../hal/hal.hpp"
//#include <lib/ORB_SLAM2/include/System.h>
#include <src/tracking/CascadeDetector.h>
#include <src/tracking/C4Detector.h>
#include <src/tracking/CombC4CascadeDetector.h>
#include <src/config/ConfigKeys.h>


class PatrolAndFollow : public BodyTest {

    typedef struct DirectionTime {
        DirectionTime():x(0),y(0),z(0),h(0){};
        DirectionTime(double x, double y, double z):x(x), y(y),z(0), h(1){}
        DirectionTime(double x, double y, double z, double h):x(x), y(y) ,z(z), h(h){}
        double x;
        double y;
        double z;
        double h;
    } DirectionTime;

    //PATRON DE VIGILANCIA
    std::vector<DirectionTime> sequence = {DirectionTime(0,0,-1,0),
                                           DirectionTime(2,0,0,0),
                                           DirectionTime(0,0,0,-1.5708),
                                           DirectionTime(0,0,0,4.71239),
                                           DirectionTime(4,0,0,0),
                                           DirectionTime(0,0,0,-1.5708),
                                           DirectionTime(0,0,0,4.71239),
                                           DirectionTime(2,0,0,0)
    };


    Hal* hal;

    bool waitingTakeOff = false;
    bool waitingLanding = false;
    bool tookOff = false;
    bool rmovemode = false;

    DetectionAlgorithm* detector;
    TrackingAlgorithm* tracker;
    DetectAndTrack* detectAndTrack;
    Follower* follower;

    bool inmc = false;

    VisualDebugger* visualDebugger;

    double targetAltitude = 3;
    double altitudeSlowdownRadius = 1;

    double currentTime = 0;

    void InitBodyTest(Hal *hal, Config* config, VisualDebugger* visualDebugger) override {

        this->hal = hal;

        //detector = new CascadeDetector();
        //detector = new HogDetector();
        //detector = new C4Detector();
        detector = new CombC4CascadeDetector();

        tracker = new MultiTracker(MultiTracker::Algorithm::KCF);
        detectAndTrack =  new DetectAndTrack(detector, tracker);
        follower = new Follower(config);

        std::shared_ptr<cv::Mat> frame = hal->getFrame(Camera::Front);
        config->Set(ConfigKeys::Drone::FrameSize, cv::Size(frame->size().width,frame->size().height));

        this->visualDebugger = visualDebugger;
    }

    bool BodyTestStep(double deltaTime) override {

        std::shared_ptr<cv::Mat> frame = hal->getFrame(Camera::Front);
        visualDebugger->setSubStatus("");

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

            //ATERRIZADO
        } else if (waitingLanding && hal->getState() == State::Landed) {
            // Aterrizado
            Logger::logError("Aterrizado");
            visualDebugger->writeConsole("Aterrizado");
            return false;

            //ATERRIZAR
        } else if (false  && sequence.size() == 0 && rmovemode && !hal->isRmoving() && !waitingLanding) {
            // Aterrizar
            Logger::logError("Aterrizar");
            visualDebugger->writeConsole("Aterrizar");
            hal->land();
            waitingLanding = true;
            return true;

            //MOVERSE
        } else {

            currentTime += deltaTime;
            FollowCommand followCommand;

            if (frame != NULL) {

                std::vector<Track> objects = detectAndTrack->update(frame);
                visualDebugger->setTracks(objects);

                if (objects.empty())
                    follower->stopFollowing();
                else if (!follower->isFollowing())
                    follower->setFollowee(objects.front().getNumber());

                followCommand = follower->follow(objects, hal->getAltitude(), deltaTime);

                visualDebugger->drawHorizon(follower->getHorizon());

                if (follower->isFollowing()) {
                    visualDebugger->setFollowCommand(followCommand);
                }

            }

            //FIN DE MOVIMIENTO
            if (rmovemode && !hal->isRmoving() && !waitingLanding) {
                rmovemode = false;
            }

            if (follower->isFollowing()) {

                double currentAltitude = hal->getAltitude();
                double deltaAltitude = targetAltitude - currentAltitude;
                double gaz = std::max(-1.0, std::min(1.0, (deltaAltitude / altitudeSlowdownRadius))) * 100;

                hal->move(0, (int) (followCommand.outputDisplacement.Pitch() * 100),
                          (int) (followCommand.outputRotation.Yaw() * 100), (int) gaz);

                visualDebugger->setSubStatus("Following");

            } else if (!rmovemode && !waitingLanding && !waitingTakeOff) {

                DirectionTime directionTime = sequence.front();
                sequence.erase(sequence.begin());
                hal->rmove(directionTime.x, directionTime.y, directionTime.z, directionTime.h);
                std::cout << "move to: " << directionTime.x << " " << directionTime.y << " " << directionTime.z << " "
                          << directionTime.h << std::endl;

                rmovemode = true;
                Logger::logError("Do rmove");

                visualDebugger->setSubStatus("Patrolling");

            } else if (rmovemode) {

                if (frame != NULL) {
                    //test track begin
                    //update the tracking result
                    std::vector<Track> objects = detectAndTrack->update(frame);
                    visualDebugger->setTracks(objects);
                }

                visualDebugger->setSubStatus("Patrolling");

            }
        }

        return true;

    }

    void FinishBodyTest() override {
        if(hal->getState() != State::Landed)
            hal->land();
    }

};