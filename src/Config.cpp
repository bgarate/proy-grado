//
// Created by bruno on 13/05/17.
//

#include "Config.h"

const std::string &Config::getName() const {
    return name;
}

void Config::setName(const std::string &name) {
    Config::name = name;
}

unsigned int Config::getId() const {
    return id;
}

void Config::setId(unsigned int id) {
    Config::id = id;
}

const std::string &Config::getBrainHost() const {
    return brainHost;
}

void Config::setBrainHost(const std::string &brainHost) {
    Config::brainHost = brainHost;
}

unsigned short Config::getBrainPort() const {
    return brainPort;
}

void Config::setBrainPort(unsigned short brainPort) {
    Config::brainPort = brainPort;
}

Config::Config() {

    brainHost = "localhost";
    brainPort = 11500;
    broadcastPort = 11501;
    advertisementLapse = 5000;
    commsPort = 11502;
    pingTimeout = 10000;
    pingLapse = 5000;
    visualDebugEnabled = true;
    outputHudVideoEnabled = true;
    outputRawVideoEnabled = false;
    outputPath = "";
    realTimeVideoOutputEnabled = false;
    pingEnabled = true;
    sleepDelay = 0;

    cameraTilt = 15;
    frameSize = cv::Size(640,480);
    fov = 80;
}

unsigned short Config::getBroadcastPort() const {
    return broadcastPort;
}

void Config::setBroadcastPort(unsigned short broadcastPort) {
    Config::broadcastPort = broadcastPort;
}

int Config::getAdvertisementLapse() const {
    return advertisementLapse;
}

void Config::setAdvertisementLapse(int advertisementLapse) {
    Config::advertisementLapse = advertisementLapse;
}

HalType Config::getHalType() const {
    return halType;
}

void Config::setHalType(HalType halType) {
    Config::halType = halType;
}

unsigned short Config::getCommsPort() const {
    return commsPort;
}

void Config::setCommsPort(unsigned short commsPort) {
    Config::commsPort = commsPort;
}

unsigned int Config::getPingTimeout() const {
    return pingTimeout;
}

void Config::setPingTimeout(unsigned int pingTimeout) {
    Config::pingTimeout = pingTimeout;
}

unsigned int Config::getPingLapse() const {
    return pingLapse;
}

void Config::setPingLapse(unsigned int pingLapse) {
    Config::pingLapse = pingLapse;
}

bool Config::isVisualDebugEnabled() const {
    return visualDebugEnabled;
}

void Config::setVisualDebugEnabled(bool visualDebug) {
    Config::visualDebugEnabled = visualDebug;
}

bool Config::isOutputHudVideoEnabled() const {
    return outputHudVideoEnabled;
}

void Config::setOutputHudVideoEnabled(bool outputHudVideoEnabled) {
    Config::outputHudVideoEnabled = outputHudVideoEnabled;
}

bool Config::isOutputRawVideoEnabled() const {
    return outputRawVideoEnabled;
}

void Config::setOutputRawVideoEnabled(bool outputRawVideoEnabled) {
    Config::outputRawVideoEnabled = outputRawVideoEnabled;
}

const std::string &Config::getOutputPath() const {
    return outputPath;
}

void Config::setOutputPath(const std::string &outputPath) {
    Config::outputPath = outputPath;
}

bool Config::isRealTimeVideoOutputEnabled() const {
    return realTimeVideoOutputEnabled;
}

void Config::setRealTimeVideoOutputEnabled(bool realTimeVideoOutputEnabled) {
    Config::realTimeVideoOutputEnabled = realTimeVideoOutputEnabled;
}

bool Config::isPingEnabled() const {
    return pingEnabled;
}

void Config::setPingEnabled(bool pingEnabled) {
    Config::pingEnabled = pingEnabled;
}

double Config::getFov() const {
    return fov;
}

void Config::setFov(double fov) {
    Config::fov = fov;
}

const cv::Size &Config::getFrameSize() const {
    return frameSize;
}

void Config::setFrameSize(const cv::Size &frameSize) {
    if(frameSize.width * frameSize.width == 0)
        throw new std::runtime_error("Frame area cannot be zero");

    Config::frameSize = frameSize;
}

double Config::getFrameSizeRatio() {
    return frameSize.width/(double)frameSize.height;
}

double Config::getVerticalFov() {
    return fov / getFrameSizeRatio();
}

const double Config::getCameraTilt() const {
    return cameraTilt;
}

void Config::setCameraTilt(const double cameraTilt) {
    Config::cameraTilt = cameraTilt;
}

unsigned int Config::getSleepDelay() const {
    return sleepDelay;
}

void Config::setSleepDelay(unsigned int sleepDelay) {
    this->sleepDelay = sleepDelay;
}



