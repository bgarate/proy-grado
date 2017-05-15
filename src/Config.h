//
// Created by bruno on 13/05/17.
//

#ifndef PROY_GRADO_CONFIG_H
#define PROY_GRADO_CONFIG_H

#include <src/hal/hal.hpp>
#include "iostream"

class Config {

private:
    std::string name;
    unsigned int id;

    std::string brainHost;
    unsigned short brainPort;

    unsigned short broadcastPort;

    int advertisementLapse;

    HalType halType;
public:
    HalType getHalType() const;

    void setHalType(HalType halType);

public:

    Config();

    const std::string &getName() const;

    void setName(const std::string &name);

    unsigned int getId() const;

    void setId(unsigned int id);

    const std::string &getBrainHost() const;

    void setBrainHost(const std::string &bodyHost);

    unsigned short getBrainPort() const;

    void setBrainPort(unsigned short bodyPort);

    unsigned short getBroadcastPort() const;

    void setBroadcastPort(unsigned short broadcastPort);

    int getAdvertisementLapse() const;

    void setAdvertisementLapse(int advertisementLapse);


};


#endif //PROY_GRADO_CONFIG_H
