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

    config->Set(ConfigKeys::Communications::BroadcastPort, 11501);
    config->Set(ConfigKeys::Communications::StateSendLapse, 300);
    config->Set(ConfigKeys::Communications::StateExpireLapse, 5000);

    config->Set(ConfigKeys::Debugging::VisualDebugEnabled, true);
    config->Set(ConfigKeys::Debugging::OutputHudVideoEnabled, true);
    config->Set(ConfigKeys::Debugging::OutputRawVideoEnabled, false);
    config->Set(ConfigKeys::Debugging::OutputPath, std::string(""));
    config->Set(ConfigKeys::Debugging::RealTimeVideoOutputEnabled, false);
    config->Set(ConfigKeys::Debugging::MapDebuggerScale, 50);
    config->Set(ConfigKeys::Debugging::MapDebuggerEnabled, true);

    config->Set(ConfigKeys::Body::SleepDelay, 0);
    config->Set(ConfigKeys::Body::Start, true);
    config->Set(ConfigKeys::Body::Hal,HalType::Dummy);
    config->Set(ConfigKeys::Body::TestToExecute,std::string("TrackMarkers"));
    config->Set(ConfigKeys::Body::DummyCameraVideoSource,std::string("-1")); // ../sample-input/drone2.mp4
    config->Set(ConfigKeys::Body::CascadeDetector,std::string("../resources/pedestrian_cascade_web_LBP.xml"));
    config->Set(ConfigKeys::Body::TrackingSmoothingSamples, 10);
    config->Set(ConfigKeys::Body::VirtualBody, false);
    config->Set(ConfigKeys::Body::Exposure, 0.0f);
    config->Set(ConfigKeys::Body::Saturation, 0.0f);
    config->Set(ConfigKeys::Body::WhiteBalance, WhiteBalanceMode::Auto);

    config->Set(ConfigKeys::Brain::Start, true);
    config->Set(ConfigKeys::Brain::LowBatteryLevel, 20);
    config->Set(ConfigKeys::Brain::CriticalBatteryLevel, 10);

    World world = getWorld(config);
    config->SetWorld(world);

    std::map<int, Path> paths = getPath(config);
    config->SetPath(paths);
}

World ConfigDefaults::getWorld(Config *config) {
    World world;

    world.addMarker(cv::Vec3d(0, 0, 0), cv::Vec3d(0, 0, 0), 1, "");
    world.addMarker(cv::Vec3d(0, 1, 0), cv::Vec3d(0, 0, 0), 2, "");
    world.addMarker(cv::Vec3d(0, 2, 0), cv::Vec3d(0, 0, 0), 3, "");
    world.addMarker(cv::Vec3d(1, 0, 0), cv::Vec3d(0, 0, 0), 4, "");
    world.addMarker(cv::Vec3d(1, 1, 0), cv::Vec3d(0, 0, 0), 5, "");
    world.addMarker(cv::Vec3d(1, 2, 0), cv::Vec3d(0, 0, 0), 6, "");
    world.addMarker(cv::Vec3d(2, 0, 0), cv::Vec3d(0, 0, 0), 7, "");
    world.addMarker(cv::Vec3d(2, 1, 0), cv::Vec3d(0, 0, 0), 8, "");
    world.addMarker(cv::Vec3d(2, 2, 0), cv::Vec3d(0, 0, 0), 9, "");
    world.addMarker(cv::Vec3d(3, 0, 0), cv::Vec3d(0, 0, 0), 10, "");
    world.addMarker(cv::Vec3d(3, 1, 0), cv::Vec3d(0, 0, 0), 11, "");
    world.addMarker(cv::Vec3d(3, 2, 0), cv::Vec3d(0, 0, 0), 12, "");
    world.addMarker(cv::Vec3d(4, 0, 0), cv::Vec3d(0, 0, 0), 13, "");
    world.addMarker(cv::Vec3d(4, 1, 0), cv::Vec3d(0, 0, 0), 14, "");
    world.addMarker(cv::Vec3d(4, 2, 0), cv::Vec3d(0, 0, 0), 15, "");
    world.addMarker(cv::Vec3d(-1, 0, 0), cv::Vec3d(0, 0, 0), 16, "");
    world.addMarker(cv::Vec3d(-1, 1, 0), cv::Vec3d(0, 0, 0), 17, "");
    world.addMarker(cv::Vec3d(-1, 2, 0), cv::Vec3d(0, 0, 0), 18, "");
    world.addMarker(cv::Vec3d(5, 0, 0), cv::Vec3d(0, 0, 0), 19, "");
    world.addMarker(cv::Vec3d(5, 1, 0), cv::Vec3d(0, 0, 0), 20, "");
    world.addMarker(cv::Vec3d(5, 2, 0), cv::Vec3d(0, 0, 0), 21, "");

    world.addPad(cv::Vec3d(2, -1, 0), cv::Vec3d(0, 0, 0), 1);

    world.addDrone(cv::Vec3d(2, 3, 0), cv::Vec3d(0, 0, 180), config->Get(ConfigKeys::Drone::Id), "");

    return world;
}

std::map<int, Path> ConfigDefaults::getPath(Config *config) {

    Path path1;

    path1.AddPoint(cv::Vec3d(3, 0, 0), 90);
    path1.AddPoint(cv::Vec3d(4, 0, 0), 90);
    path1.AddPoint(cv::Vec3d(5, 0, 0), 0);
    path1.AddPoint(cv::Vec3d(5, 1, 0), 0);
    path1.AddPoint(cv::Vec3d(5, 2, 0), -90);
    path1.AddPoint(cv::Vec3d(4, 2, 0), -90);
    path1.AddPoint(cv::Vec3d(3, 2, 0), -180);
    path1.AddPoint(cv::Vec3d(3, 1, 0), -180);

    Path path2;

    path2.AddPoint(cv::Vec3d(3, 0, 0), 90);
    path2.AddPoint(cv::Vec3d(4, 0, 0), 90);
    path2.AddPoint(cv::Vec3d(5, 0, 0), 0);
    path2.AddPoint(cv::Vec3d(5, 1, 0), 0);
    path2.AddPoint(cv::Vec3d(5, 2, 0), -90);
    path2.AddPoint(cv::Vec3d(4, 2, 0), -90);
    path2.AddPoint(cv::Vec3d(3, 2, 0), -90);
    path2.AddPoint(cv::Vec3d(2, 2, 0), -90);
    path2.AddPoint(cv::Vec3d(1, 2, 0), -90);
    path2.AddPoint(cv::Vec3d(0, 2, 0), -90);
    path2.AddPoint(cv::Vec3d(-1, 2, 0), -180);
    path2.AddPoint(cv::Vec3d(-1, 1, 0), -180);
    path2.AddPoint(cv::Vec3d(-1, 0, 0), 90);
    path2.AddPoint(cv::Vec3d(0, 0, 0), 90);
    path2.AddPoint(cv::Vec3d(1, 0, 0), 90);
    path2.AddPoint(cv::Vec3d(2, 0, 0), 90);

    std::map<int, Path> paths;
    paths[1] = path1;
    paths[2] = path2;

    return paths;
}
