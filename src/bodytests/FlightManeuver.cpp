//
// Created by bruno on 18/05/17.
//

#include <zconf.h>
#include <src/logging/Logger.h>
#include <src/tracking/DetectAndTrack.h>
#include <src/tracking/HogDetector.h>
#include <src/tracking/MultiTracker.h>
#include "BodyTest.h"
#include "../hal/hal.hpp"

class FlightManeuver : public BodyTest {

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

    std::vector<DirectionTime> sequence = {DirectionTime(0,0,0,1000)};

    bool waitingTakeOff = false;
    bool waitingLanding = false;
    bool tookOff = false;

    cv::Scalar colors[9];
    DetectionAlgorithm* detector;
    TrackingAlgorithm* tracker;
    DetectAndTrack* detectAndTrack;

    void InitBodyTest(Hal *hal) override {
        this->hal = hal;

        colors[0] = cv::Scalar(255,0,0);
        colors[1] = cv::Scalar(0,255,0);
        colors[2] = cv::Scalar(0,0,255);
        colors[3] = cv::Scalar(120,120,0);
        colors[4] = cv::Scalar(0,120,120);
        colors[5] = cv::Scalar(120,0,120);
        colors[6] = cv::Scalar(255,0,120);
        colors[7] = cv::Scalar(0,120,255);
        colors[8] = cv::Scalar(255,120,0);
        detector = new HogDetector();
        tracker = new MultiTracker(MultiTracker::Algorithm::KCF);
        detectAndTrack =  new DetectAndTrack(detector, tracker);

        cv::namedWindow("tracker", cv::WINDOW_AUTOSIZE);

        Logger::logInfo("Bateria: %u") << hal->bateryLevel();
    }

    bool BodyTestStep(double deltaTime) override {

        if(hal->getState() == State::Landed && !tookOff){
            // Despegar
            Logger::logError("Despegar");
            hal->takeoff();
            tookOff = true;
            waitingTakeOff = true;
            return true;
        } else if(waitingTakeOff &&
                (hal->getState() == State::Hovering || hal->getState() == State::Flying))
        {
            Logger::logError("Despegado");
            // Despegado
            waitingTakeOff = false;
        } else if(waitingLanding && hal->getState() == State::Landed){
            // Aterrizado
            Logger::logError("Aterrizado");
            return false;
        } else if(currentStep >= sequence.size() && !waitingLanding){
            // Aterrizar
            Logger::logError("Aterrizar");
            hal->land();
            waitingLanding = true;
            return true;
        } else if(!waitingLanding && !waitingTakeOff) {


            DirectionTime directionTime = sequence[currentStep];

            hal->move((int) (directionTime.y * 100), (int) (directionTime.x * 100),
                      0, (int) (directionTime.z) * 100);

            if (currentTime >= directionTime.s * 1000000) {
                currentTime = 0;
                currentStep++;
                Logger::logInfo("Next element in flight maneuver sequence");
            } else {
                currentTime += deltaTime;
            }

            std::shared_ptr<cv::Mat> frame = hal->getFrame(Camera::Front);
            if(frame != NULL){

                //test track begin
                //update the tracking result
                std::vector<Track> objects = detectAndTrack->update(*frame);

                // draw the tracked object
                for (unsigned i = 0; i < objects.size(); i++)
                    rectangle(*frame, objects[i].getRect(), colors[objects[i].getNumber() % (sizeof(colors)/sizeof(cv::Scalar))], 2, 1);


                cv::imshow("tracker", *frame);
                if(cv::waitKey(1) == 27){
                    return false;
                }
            }
        }
        return true;

    }

    void FinishBodyTest() override {
        if(hal->getState() != State::Landed)
            hal->land();
    }

};