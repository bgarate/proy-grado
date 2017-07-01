//
// Created by bruno on 13/05/17.
//

#ifndef PROY_GRADO_CONFIG_H
#define PROY_GRADO_CONFIG_H

#include <opencv2/core/types.hpp>
#include "iostream"
#include "hal/HalType.hpp"

class Config {

public:
    const std::string &getOutputPath() const;

    void setOutputPath(const std::string &outputPath);

    bool isOutputHudVideoEnabled() const;

    void setOutputHudVideoEnabled(bool outputHudVideo);

    bool isOutputRawVideoEnabled() const;

    void setOutputRawVideoEnabled(bool outputRawVideo);

    bool isRealTimeVideoOutputEnabled() const;

    void setRealTimeVideoOutputEnabled(bool realTimeVideoOutput);

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

    bool isPingEnabled() const;

    void setPingEnabled(bool pingEnabled);

    double getFov() const;

    void setFov(double fov);

    const cv::Size &getFrameSize() const;

    void setFrameSize(const cv::Size &frameSize);

    double getFrameSizeRatio();

    double getVerticalFov();

    /**
     * Gets the Bebop 2 drone physical camera tilt. It doesnt takes into account
     * software tilt. It is taken as the vertical angle between the camera orientation
     * and the horizontal plane.
     * @return
     */
    const double getCameraTilt() const;

    void setCameraTilt(const double cameraTilt);


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

    bool outputHudVideoEnabled;

    bool realTimeVideoOutputEnabled;

    bool outputRawVideoEnabled;

    std::string outputPath;

    bool pingEnabled;

    double fov;

    cv::Size frameSize;

    double cameraTilt;

};


#endif //PROY_GRADO_CONFIG_H
