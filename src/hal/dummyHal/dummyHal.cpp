//
// Created by bruno on 14/05/17.
//

#include "../hal.hpp"
#include <memory.h>
#include "dummyHal.h"
#include "../Point.h"

DummyHal::DummyHal():videoSource("markertest.mp4") {
    state = State::Landed;
}

void DummyHal::move(int roll, int pitch, int yaw, int gaz) {
    return;
}

void DummyHal::setup(Config* config) {
    this->config = config;
}

void DummyHal::land() {
    state = State::Landed;
}

void DummyHal::takeoff() {
    state = State::Flying;
}

int DummyHal::bateryLevel() {
    return 100;
}

void DummyHal::setCameraTilt(Camera cam){

}

std::shared_ptr<cv::Mat> DummyHal::getFrame(Camera cam) {
    cv::Mat frame;
    videoSource >> frame;
    if(frame.empty()) {
        videoSource.set(cv::CAP_PROP_POS_FRAMES, 0);
        videoSource >> frame;
    }
    return std::shared_ptr<cv::Mat>(new cv::Mat(frame));
}

double DummyHal::getAltitude() {
    return 5;
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

void DummyHal::rmove(double dx, double dy, double dz, double dh) {

};

State DummyHal::getState() {
    return state;
}

bool DummyHal::isRmoving(){
    //todo
    return false;
}


