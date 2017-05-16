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
