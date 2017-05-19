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
        DirectionTime(double x, double y, double z):x(x), y(y),z(0), s(1000){}
        DirectionTime(double x, double y, double z, double s):x(x), y(y),z(0), s(s){}
        double x;
        double y;
        double z;
        double s;
    } DirectionTime;


    Hal* hal;
    int currentStep = 0;
    double currentTime = 0;
    std::vector<DirectionTime> sequence = {DirectionTime(-0.25,0,2), DirectionTime(0,-0.25,2),
                                DirectionTime(0.25,0,2),DirectionTime(0,0.25,2)};

    void InitBodyTest(Hal *hal) override {
        this->hal = hal;
        cv::namedWindow("Bebop", cv::WINDOW_AUTOSIZE);
        Logger::logInfo("Pre takeoff");
        hal->takeoff();
        Logger::logInfo("Post takeoff");
    }

    bool BodyTestStep(double deltaTime) override {

        DirectionTime directionTime = sequence[currentStep];

        hal->move((int)(directionTime.y * 100),(int)(directionTime.x * 100),
                  0,(int)(directionTime.z) * 100);

        if(currentTime >= directionTime.s * 1000000) {
            currentTime = 0;
            currentStep++;
            Logger::logCritical("Changed!");
        } else {
            currentTime += deltaTime;
            Logger::logError(std::to_string(currentTime));
        }

        cv::Mat* frame = hal->getFrame(Camera::Front);
        /*cv::imshow("tracker", *frame);*/

        return currentStep < sequence.size();

    }

    void FinishBodyTest() override {
        hal->land();
    }

};