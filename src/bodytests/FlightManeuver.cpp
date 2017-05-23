//
// Created by bruno on 18/05/17.
//

#include <zconf.h>
#include <src/logging/Logger.h>
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
    std::vector<DirectionTime> sequence = {DirectionTime(-0.25,0,0,2), DirectionTime(0,-0.25,0,2),
                                DirectionTime(0.25,0,0,2),DirectionTime(0,0.25,0,2)};

    bool waitingTakeOff = false;
    bool waitingLanding = false;
    bool tookOff = false;

    void InitBodyTest(Hal *hal) override {
        this->hal = hal;
        cv::namedWindow("Bebop", cv::WINDOW_AUTOSIZE);

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

            cv::Mat *frame = hal->getFrame(Camera::Front);
            if(frame != NULL){
                cv::imshow("tracker", *frame);
                cv::waitKey(1);
            }
        }
        return true;

    }

    void FinishBodyTest() override {
        //hal->land();
    }

};