//
// Created by bruno on 13/05/17.
//

#ifndef PROY_GRADO_CONFIG_H
#define PROY_GRADO_CONFIG_H

#include <src/hal/HalType.hpp>
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

    unsigned short commsPort;

    unsigned int pingTimeout;

    unsigned int pingLapse;

    bool visualDebugEnabled;

    bool outputHudVideo;

    bool realTimeVideoOutput;
public:
    bool isRealTimeVideoOutput() const;

    void setRealTimeVideoOutput(bool realTimeVideoOutput);

private:

    std::string outputPath;
public:
    const std::string &getOutputPath() const;

    void setOutputPath(const std::string &outputPath);

public:
    bool isOutputHudVideo() const;

    void setOutputHudVideo(bool outputHudVideo);

    bool isOutputRawVideo() const;

    void setOutputRawVideo(bool outputRawVideo);

private:
    bool outputRawVideo;

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

    unsigned short getCommsPort() const;

    void setCommsPort(unsigned short commsPort);

    HalType getHalType() const;

    void setHalType(HalType halType);

    unsigned int getPingTimeout() const;

    void setPingTimeout(unsigned int pingTimeout);

    unsigned int getPingLapse() const;

    void setPingLapse(unsigned int pingLapse);

    bool isVisualDebugEnabled() const;

    void setVisualDebugEnabled(bool visualDebug);

};


#endif //PROY_GRADO_CONFIG_H
