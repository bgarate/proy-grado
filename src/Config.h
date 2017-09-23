//
// Created by bruno on 13/05/17.
//

#ifndef PROY_GRADO_CONFIG_H
#define PROY_GRADO_CONFIG_H

#include <opencv2/core/types.hpp>
#include "iostream"
#include "hal/HalType.hpp"
#include <opencv2/opencv.hpp>
#include <yaml-cpp/yaml.h>

#define CONFIG_KEY(PARENT, KEY) static constexpr const ConfigKey KEY = ConfigKey(#KEY,#PARENT);

struct ConfigKey {
public:
    constexpr ConfigKey(const char* key, const char* parent):Key(key),Parent(parent){}
    const char* Key;
    const char* Parent;
};

struct ConfigKeys {

    struct Drone {
        CONFIG_KEY(Drone, Name)
        CONFIG_KEY(Drone, Id)
        CONFIG_KEY(Drone, HalType)
        CONFIG_KEY(Drone, FOV)
        CONFIG_KEY(Drone, FrameSize)
        CONFIG_KEY(Drone, CameraTilt)
        CONFIG_KEY(Drone, SleepDelay)
        CONFIG_KEY(Drone, CameraMatrix)
        CONFIG_KEY(Drone, DistortionCoefficients)
    };

    struct Communications {
        CONFIG_KEY(Communications, BrainHost)
        CONFIG_KEY(Communications, BrainPort)
        CONFIG_KEY(Communications, BroadcastPort)
        CONFIG_KEY(Communications, AdvertisementLapse)
        CONFIG_KEY(Communications, CommunicationPort)
        CONFIG_KEY(Communications, PingTimeout)
        CONFIG_KEY(Communications, PingLapse)
        CONFIG_KEY(Communications, PingEnabled);
    };

    struct Debugging {
        CONFIG_KEY(Debugging, VisualDebugEnabled);
        CONFIG_KEY(Debugging, OutputHudVideoEnabled);
        CONFIG_KEY(Debugging, RealTimeVideoOutputEnabled);
        CONFIG_KEY(Debugging, OutputRawVideoEnabled);
        CONFIG_KEY(Debugging, OutputPath);
    };

    struct Body {
        CONFIG_KEY(Body, SleepDelay)
    };
};

class Config {

public:

    Config():path("config.yaml"){}

    void SetPath(const std::string &path) {
        this->path = std::string(path);
    }

    template <typename T>
    void Set(const ConfigKey &key, T value) {
        config[std::string(key.Parent)][std::string(key.Key)] = value;
    }

    template <typename T>
    T Get(const ConfigKey &key) {
        return config[std::string(key.Parent)][std::string(key.Key)].as<T>();
    }

    void Load() {

        std::ifstream file(path);

        if(file) {
            SetDefaults();
            Save();
        }

        config = YAML::LoadFile(path);
    }

    void Save() {
        std::ofstream file(path);
        file << config;
        file.close();
    }

private:
    YAML::Node config;
    std::string path;

    void Get(const ConfigKey &key);

    void SetDefaults() {
        YAML::Node node = YAML::Node();

        Set(ConfigKeys::Drone::CameraTilt, 15);
        Set(ConfigKeys::Drone::FrameSize, cv::Size(640,480));
        Set(ConfigKeys::Drone::FOV, 80);

        Set(ConfigKeys::Drone::CameraMatrix, 80);
        Set(ConfigKeys::Drone::DistortionCoefficients, 80);

        Set(ConfigKeys::Communications::BrainHost, "localhost");
        Set(ConfigKeys::Communications::BrainPort, 11500);
        Set(ConfigKeys::Communications::BroadcastPort, 11501);
        Set(ConfigKeys::Communications::AdvertisementLapse, 5000);
        Set(ConfigKeys::Communications::CommunicationPort, 11502);
        Set(ConfigKeys::Communications::PingTimeout, 10000);
        Set(ConfigKeys::Communications::PingLapse, 5000);
        Set(ConfigKeys::Communications::PingEnabled, false);

        Set(ConfigKeys::Debugging::VisualDebugEnabled, true);
        Set(ConfigKeys::Debugging::OutputHudVideoEnabled, true);
        Set(ConfigKeys::Debugging::OutputRawVideoEnabled, false);
        Set(ConfigKeys::Debugging::OutputPath, "");
        Set(ConfigKeys::Debugging::RealTimeVideoOutputEnabled, false);

        Set(ConfigKeys::Body::SleepDelay, 0);

    }
};

#endif //PROY_GRADO_CONFIG_H
