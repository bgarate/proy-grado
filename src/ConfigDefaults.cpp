//
// Created by bruno on 24/09/17.
//

#include "ConfigDefaults.h"
#include "ConfigKeys.h"
#include "opencv2/opencv.hpp"

void ConfigDefaults::SetDefaults(Config* config) {

    config->Set(ConfigKeys::Drone::Name, std::string("drone-01"));
    config->Set(ConfigKeys::Drone::Id, 1);
    config->Set(ConfigKeys::Drone::CameraTilt, 15.0);
    config->Set(ConfigKeys::Drone::FOV, 80.0);
    config->Set(ConfigKeys::Drone::FrameSize, cv::Size(640,480));

    double cameraMatrix[] = {
            9.0349677104287230e+02, 0., 3.1801716245770621e+02,
            0., 9.0349677104287230e+02, 1.0339929044950360e+02,
            0., 0., 1.
    };

    double distortionCoefficients[] = {
            4.8729805206966542e-01, -1.1441028348243871e+00, -1.0495937548646615e-01,
            2.7894082581863826e-03, 2.4154637069022185e+00
    };

    config->Set(ConfigKeys::Drone::CameraMatrix, cv::Mat(3,3,CV_32F,cameraMatrix));
    config->Set(ConfigKeys::Drone::DistortionCoefficients, cv::Mat(5,1,CV_32F,distortionCoefficients));

    config->Set(ConfigKeys::Communications::BrainHost, std::string("localhost"));
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
    config->Set(ConfigKeys::Debugging::OutputPath, std::string(""));
    config->Set(ConfigKeys::Debugging::RealTimeVideoOutputEnabled, false);

    config->Set(ConfigKeys::Body::SleepDelay, 0);
    config->Set(ConfigKeys::Body::Start, true);
    config->Set(ConfigKeys::Body::Hal,HalType::Dummy);
    config->Set(ConfigKeys::Body::ParentOnFork,true);
    config->Set(ConfigKeys::Body::TestToExecute,std::string("OpticalFlowObstacleAvoidance"));
    config->Set(ConfigKeys::Body::DummyCameraVideoSource,std::string("drone.mp4"));

    config->Set(ConfigKeys::Brain::Start, true);

}
