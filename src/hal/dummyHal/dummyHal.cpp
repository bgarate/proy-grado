//
// Created by bruno on 14/05/17.
//

#include <src/hal/hal.hpp>
#include "dummyHal.h"

void DummyHal::move(int roll, int pitch, int yaw, int gaz) {
    return;
}

void DummyHal::land() {
    return;
}

void DummyHal::takeoff() {
    return;
}

int DummyHal::bateryLevel() {
    return 100;
}

cv::Mat *DummyHal::getFrame(Camera cam) {
    return nullptr;
}

double DummyHal::getAltitude() {
    return 1;
}

Point DummyHal::getOrientation() {
    return Point(1,1,1);
}

Point DummyHal::getGPSPosition() {
    return Point(1,1,1);
}


void DummyHal::Connect() {

}

void DummyHal::Disconnect(){

}

