//
// Created by bruno on 24/09/17.
//

#ifndef PROY_GRADO_CONFIGKEYS_H
#define PROY_GRADO_CONFIGKEYS_H


#include "../config/Config.h"
#include "../hal/HalType.hpp"

struct ConfigKeys {

    struct Drone {
        CONFIG_KEY(Drone, Name, std::string)
        CONFIG_KEY(Drone, Id, int)
        CONFIG_KEY(Drone, FOV, double)
        CONFIG_KEY(Drone, FrameSize, cv::Size)
        CONFIG_KEY(Drone, CameraTilt, double)
        CONFIG_KEY(Drone, CameraMatrix, cv::Mat)
        CONFIG_KEY(Drone, DistortionCoefficients, cv::Mat)

        CONFIG_KEY(Drone, FrameSizeRatio, double)

        CONFIG_KEY(Drone, VerticalFOV, double)
    };

    struct Communications {
        CONFIG_KEY(Communications, BrainHost, std::string)
        CONFIG_KEY(Communications, BrainPort, int)
        CONFIG_KEY(Communications, BroadcastPort, int)
        CONFIG_KEY(Communications, AdvertisementLapse, int)
        CONFIG_KEY(Communications, CommunicationPort, int)
        CONFIG_KEY(Communications, PingTimeout, int)
        CONFIG_KEY(Communications, PingLapse, int)
        CONFIG_KEY(Communications, PingEnabled, bool);
        CONFIG_KEY(Communications, StateSendLapse, int);
        CONFIG_KEY(Communications, StateExpireLapse, int);
    };

    struct Debugging {
        CONFIG_KEY(Debugging, VisualDebugEnabled, bool);
        CONFIG_KEY(Debugging, OutputHudVideoEnabled, bool);
        CONFIG_KEY(Debugging, RealTimeVideoOutputEnabled, bool);
        CONFIG_KEY(Debugging, OutputRawVideoEnabled, bool);
        CONFIG_KEY(Debugging, OutputPath, std::string);
        CONFIG_KEY(Debugging, NavigationDebuggerScale, int);
    };

    struct Body {
        CONFIG_KEY(Body, SleepDelay, int)
        CONFIG_KEY(Body, Hal, HalType)
        CONFIG_KEY(Body, Start, bool)
        CONFIG_KEY(Body, ParentOnFork, bool)
        CONFIG_KEY(Body, TestToExecute, std::string)
        CONFIG_KEY(Body, DummyCameraVideoSource, std::string)
        CONFIG_KEY(Body, CascadeDetector, std::string)
        CONFIG_KEY(Body, TrackingSmoothingSamples, int)
    };

    struct Brain {
        CONFIG_KEY(Body, Start, bool)
    };
};

#include "Config.h"

#endif //PROY_GRADO_CONFIGKEYS_H
