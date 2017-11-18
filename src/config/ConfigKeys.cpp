//
// Created by bruno on 24/09/17.
//

#include "ConfigKeys.h"

DEFINE_CONFIG_KEY(Drone, Name, std::string)
DEFINE_CONFIG_KEY(Drone, Id, int)
DEFINE_CONFIG_KEY(Drone, FOV, double)
DEFINE_CONFIG_KEY(Drone, FrameSize, cv::Size)
DEFINE_CONFIG_KEY(Drone, CameraTilt, double)
DEFINE_CONFIG_KEY(Drone, CameraMatrix, cv::Mat)
DEFINE_CONFIG_KEY(Drone, DistortionCoefficients, cv::Mat)

DEFINE_CONFIG_KEY_CALCULATED(Drone, FrameSizeRatio, double, [](
Config* config) -> double {
    cv::Size size = config->Get<cv::Size>(ConfigKeys::Drone::FrameSize);
    return size.width/(double)size.height;
})

DEFINE_CONFIG_KEY_CALCULATED(Drone, VerticalFOV, double, [](
Config* config) -> double {
    double aspect = config->Get<double>(ConfigKeys::Drone::FrameSizeRatio);
    double fov = config->Get<double>(ConfigKeys::Drone::FOV);
return fov/aspect;
})

DEFINE_CONFIG_KEY(Communications, BrainHost, std::string)
DEFINE_CONFIG_KEY(Communications, BrainPort, int)
DEFINE_CONFIG_KEY(Communications, BroadcastPort, int)
DEFINE_CONFIG_KEY(Communications, AdvertisementLapse, int)
DEFINE_CONFIG_KEY(Communications, CommunicationPort, int)
DEFINE_CONFIG_KEY(Communications, PingTimeout, int)
DEFINE_CONFIG_KEY(Communications, PingLapse, int)
DEFINE_CONFIG_KEY(Communications, PingEnabled, bool);

DEFINE_CONFIG_KEY(Debugging, VisualDebugEnabled, bool);
DEFINE_CONFIG_KEY(Debugging, OutputHudVideoEnabled, bool);
DEFINE_CONFIG_KEY(Debugging, RealTimeVideoOutputEnabled, bool);
DEFINE_CONFIG_KEY(Debugging, OutputRawVideoEnabled, bool);
DEFINE_CONFIG_KEY(Debugging, OutputPath, std::string);

DEFINE_CONFIG_KEY(Body, SleepDelay, int)
DEFINE_CONFIG_KEY(Body, Hal, HalType)
DEFINE_CONFIG_KEY(Body, Start, bool)
DEFINE_CONFIG_KEY(Body, ParentOnFork, bool)
DEFINE_CONFIG_KEY(Body, TestToExecute, std::string)
DEFINE_CONFIG_KEY(Body, DummyCameraVideoSource, std::string)
DEFINE_CONFIG_KEY(Body, CascadeDetector, std::string)
DEFINE_CONFIG_KEY(Body, TrackingSmoothingSamples, int)

DEFINE_CONFIG_KEY(Brain, Start, bool)
