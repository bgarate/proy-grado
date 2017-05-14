//
// Created by bruno on 13/05/17.
//

#ifndef PROY_GRADO_CONFIG_H
#define PROY_GRADO_CONFIG_H

#include "iostream"

class Config {

public:

    Config();

    const std::string &getName() const;

    void setName(const std::string &name);

    unsigned int getId() const;

    void setId(unsigned int id);

    const std::string &getBodyHost() const;

    void setBodyHost(const std::string &bodyHost);

    unsigned short getBodyPort() const;

    void setBodyPort(unsigned short bodyPort);

    unsigned short getBroadcastPort() const;

    void setBroadcastPort(unsigned short broadcastPort);


private:
    std::string name;
    unsigned int id;

    std::string bodyHost;
    unsigned short bodyPort;

    unsigned short broadcastPort;

   int advertisementLapse;
public:
    int getAdvertisementLapse() const;

    void setAdvertisementLapse(int advertisementLapse);

};


#endif //PROY_GRADO_CONFIG_H
