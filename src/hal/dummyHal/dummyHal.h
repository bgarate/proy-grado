//
// Created by bruno on 14/05/17.
//

#ifndef PROY_GRADO_DUMMYHAL_H
#define PROY_GRADO_DUMMYHAL_H


#include <src/hal/hal.hpp>

class DummyHal: public Hal {
    void move(int roll, int pitch, int yaw, int gaz) override;

    void land() override;

    void takeoff() override;

    int bateryLevel() override;

    cv::Mat *getFrame(Camera cam) override;

    double getAltitude() override;

    Point getOrientation() override;

    Point getGPSPosition() override;

    void Disconnect() override;

    void Connect() override;

    State getState() override;

};


#endif //PROY_GRADO_DUMMYHAL_H
