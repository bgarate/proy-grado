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

const std::string &Config::getBodyHost() const {
    return bodyHost;
}

void Config::setBodyHost(const std::string &bodyHost) {
    Config::bodyHost = bodyHost;
}

unsigned short Config::getBodyPort() const {
    return bodyPort;
}

void Config::setBodyPort(unsigned short bodyPort) {
    Config::bodyPort = bodyPort;
}

Config::Config() {

    name = "PB2";
    id = 0;
    bodyHost = "localhost";
    bodyPort = 11500;
    broadcastPort = 11501;
    advertisementLapse = 5000;

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
