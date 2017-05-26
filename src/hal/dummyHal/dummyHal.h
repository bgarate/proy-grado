//
// Created by bruno on 14/05/17.
//

#ifndef PROY_GRADO_DUMMYHAL_H
#define PROY_GRADO_DUMMYHAL_H


#include <src/hal/hal.hpp>

class DummyHal: public Hal {
public:
    DummyHal();

    void move(int roll, int pitch, int yaw, int gaz) override;

    void land() override;

    void takeoff() override;

    int bateryLevel() override;

    std::shared_ptr<cv::Mat> getFrame(Camera cam) override;

    double getAltitude() override;

    Point getOrientation() override;

    Point getGPSPosition() override;

    void Disconnect() override;

    void Connect() override;

    State getState() override;

    void rmove(double dx, double dy, double dz, double dh) override;

    void setup(Config* config) override;

private:
    State state;
    cv::VideoCapture videoSource;
    Config* config;
};


#endif //PROY_GRADO_DUMMYHAL_H
