//
// Created by bruno on 24/09/17.
//

#include "ConfigDefaults.h"
#include "ConfigKeys.h"

void ConfigDefaults::SetDefaults(Config* config) {

    config->Set(ConfigKeys::Drone::CameraTilt, 15);
    config->Set(ConfigKeys::Drone::FOV, 80);
    config->Set(ConfigKeys::Drone::FrameSize, cv::Size(640,480));

    config->Set(ConfigKeys::Drone::CameraMatrix, 80);
    config->Set(ConfigKeys::Drone::DistortionCoefficients, 80);

    config->Set(ConfigKeys::Communications::BrainHost, "localhost");
    config->Set(ConfigKeys::Communications::BrainPort, 11500);
    config->Set(ConfigKeys::Communications::BroadcastPort, 11501);
    config->Set(ConfigKeys::Communications::AdvertisementLapse, 5000);
    config->Set(ConfigKeys::Communications::CommunicationPort, 11502);
    config->Set(ConfigKeys::Communications::PingTimeout, 10000);
    config->Set(ConfigKeys::Communications::PingLapse, 5000);
    config->Set(ConfigKeys::Communications::PingEnabled, false);

    config->Set(ConfigKeys::Debugging::VisualDebugEnabled, true);
    config->Set(ConfigKeys::Debugging::OutputHudVideoEnabled, true);
    config->Set(ConfigKeys::Debugging::OutputRawVideoEnabled, false);
    config->Set(ConfigKeys::Debugging::OutputPath, "");
    config->Set(ConfigKeys::Debugging::RealTimeVideoOutputEnabled, false);

    config->Set(ConfigKeys::Body::SleepDelay, 0);

}
