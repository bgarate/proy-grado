//
// Created by bruno on 24/09/17.
//

#include "ConfigDefaults.h"
#include "ConfigKeys.h"

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

    config->Set(ConfigKeys::Drone::CameraMatrix, cv::Mat(3,3,CV_64F,cameraMatrix));
    config->Set(ConfigKeys::Drone::DistortionCoefficients, cv::Mat(5,1,CV_64F,distortionCoefficients));

    config->Set(ConfigKeys::Communications::BrainHost, std::string("localhost"));
    config->Set(ConfigKeys::Communications::BrainPort, 11500);
    config->Set(ConfigKeys::Communications::BroadcastPort, 11501);
    config->Set(ConfigKeys::Communications::AdvertisementLapse, 5000);
    config->Set(ConfigKeys::Communications::CommunicationPort, 11502);
    config->Set(ConfigKeys::Communications::PingTimeout, 10000);
    config->Set(ConfigKeys::Communications::PingLapse, 5000);
    config->Set(ConfigKeys::Communications::PingEnabled, false);
    config->Set(ConfigKeys::Communications::StateSendLapse, 300);
    config->Set(ConfigKeys::Communications::StateExpireLapse, 5000);

    config->Set(ConfigKeys::Debugging::VisualDebugEnabled, true);
    config->Set(ConfigKeys::Debugging::OutputHudVideoEnabled, true);
    config->Set(ConfigKeys::Debugging::OutputRawVideoEnabled, false);
    config->Set(ConfigKeys::Debugging::OutputPath, std::string(""));
    config->Set(ConfigKeys::Debugging::RealTimeVideoOutputEnabled, false);
    config->Set(ConfigKeys::Debugging::NavigationDebuggerScale, 50);

    config->Set(ConfigKeys::Body::SleepDelay, 0);
    config->Set(ConfigKeys::Body::Start, true);
    config->Set(ConfigKeys::Body::Hal,HalType::Dummy);
    config->Set(ConfigKeys::Body::ParentOnFork,true);
    config->Set(ConfigKeys::Body::TestToExecute,std::string("TrackMarkers"));
    config->Set(ConfigKeys::Body::DummyCameraVideoSource,std::string("-1")); // ../sample-input/drone2.mp4
    config->Set(ConfigKeys::Body::CascadeDetector,std::string("../resources/pedestrian_cascade_web_LBP.xml"));
    config->Set(ConfigKeys::Body::TrackingSmoothingSamples, 10);

    config->Set(ConfigKeys::Brain::Start, true);

    World world = getWorld(config);
    config->SetWorld(world);

    Path path = getPath(config);
    config->SetPath(path);
}

World ConfigDefaults::getWorld(Config *config) {
    World world;

    world.addMarker(cv::Vec3d(0, 0, 0), cv::Vec3d(0, 0, 0), 0);
    world.addMarker(cv::Vec3d(0, 2.65, 0), cv::Vec3d(0, 0, 0), 5);
    world.addMarker(cv::Vec3d(0, 2.65 + 2.05, 0), cv::Vec3d(0, 0, 0), 1);
    world.addMarker(cv::Vec3d(0, 2.65 + 2.05 + 1.56, 0), cv::Vec3d(0, 0, 90), 8);

    world.addMarker(cv::Vec3d(1.74, 2.65 + 2.05 + 1.56, 0), cv::Vec3d(0, 0, 90), 4);
    world.addMarker(cv::Vec3d(1.74 + 1.47, 2.65 + 2.05 + 1.56, 0), cv::Vec3d(0, 0, 180), 11);

    world.addMarker(cv::Vec3d(1.74 + 1.47, 1.40 + 1.40 + 1.9, 0), cv::Vec3d(0, 0, 180), 9);
    world.addMarker(cv::Vec3d(1.74 + 1.47, 1.40 + 1.40, 0), cv::Vec3d(0, 0, 180), 10);
    world.addMarker(cv::Vec3d(1.74 + 1.47, 1.40, 0), cv::Vec3d(0, 0, 180), 6);
    world.addMarker(cv::Vec3d(1.74 + 1.47, 0, 0), cv::Vec3d(0, 0, -90), 7);

    world.addMarker(cv::Vec3d(1.67, 0, 0), cv::Vec3d(0, 0, -90), 3);
    world.addMarker(cv::Vec3d(1.67, 2.75, 0), cv::Vec3d(0, 0, 0), 2);

    world.addDrone(cv::Vec3d(5, 0, 0), cv::Vec3d(0, 0, 0), config->Get(ConfigKeys::Drone::Id));
    return world;
}

Path ConfigDefaults::getPath(Config *config) {
    Path path;

    path.AddPoint(cv::Vec3d(-1.35, -1.6, 5), 45);
    path.AddPoint(cv::Vec3d(-1.2, 3.4, 5), 135);
    path.AddPoint(cv::Vec3d(3.8, 3.4, 5), -135);
    path.AddPoint(cv::Vec3d(3.4, 1.6, 5), -90);
    path.AddPoint(cv::Vec3d(5.3, 1.8, 5), -135);
    path.AddPoint(cv::Vec3d(5.6, -1.4, 5), -45);
    path.AddPoint(cv::Vec3d(3.1, -1.2, 5), 0);

    return path;
}
