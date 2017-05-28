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
    name = "PB2";
    id = 0;
    brainHost = "localhost";
    brainPort = 11500;
    broadcastPort = 11501;
    advertisementLapse = 5000;
    commsPort = 11502;
    pingTimeout = 2000;
    pingLapse = 5000;
    visualDebugEnabled = true;
    outputHudVideo = true;
    outputRawVideo = false;
    outputPath = "";
    realTimeVideoOutput = false;
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

bool Config::isOutputHudVideo() const {
    return outputHudVideo;
}

void Config::setOutputHudVideo(bool outputHudVideo) {
    Config::outputHudVideo = outputHudVideo;
}

bool Config::isOutputRawVideo() const {
    return outputRawVideo;
}

void Config::setOutputRawVideo(bool outputRawVideo) {
    Config::outputRawVideo = outputRawVideo;
}

const std::string &Config::getOutputPath() const {
    return outputPath;
}

void Config::setOutputPath(const std::string &outputPath) {
    Config::outputPath = outputPath;
}

bool Config::isRealTimeVideoOutput() const {
    return realTimeVideoOutput;
}

void Config::setRealTimeVideoOutput(bool realTimeVideoOutput) {
    Config::realTimeVideoOutput = realTimeVideoOutput;
}
