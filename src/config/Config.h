//
// Created by bruno on 13/05/17.
//

#ifndef PROY_GRADO_CONFIG_H
#define PROY_GRADO_CONFIG_H

#include <opencv2/core/types.hpp>
#include "iostream"
#include "ConfigDefaults.h"
#include <opencv2/opencv.hpp>
#include <yaml-cpp/yaml.h>
#include <src/logging/Logger.h>

#define CONFIG_KEY(PARENT, KEY, T) static const ConfigKey<T> KEY;
#define DEFINE_CONFIG_KEY(PARENT, KEY, T) const ConfigKey<T> ConfigKeys::PARENT::KEY = ConfigKey<T>(#KEY,#PARENT);
#define DEFINE_CONFIG_KEY_CALCULATED(PARENT, KEY, T, LAMBDA) const ConfigKey<T> ConfigKeys::PARENT::KEY = ConfigKey<T>(#KEY,#PARENT, LAMBDA);


class Config;

template <typename T>
struct ConfigKey {
public:

    typedef std::function<T(Config*)> CalculatedKeyDelegate;

    constexpr ConfigKey(const char* key, const char* parent):Key(key),Parent(parent){}
    constexpr ConfigKey(const char* key, const char* parent, CalculatedKeyDelegate handler):
            Key(key),Parent(parent), Handler(handler){}

    const char* Key;
    const char* Parent;
    CalculatedKeyDelegate Handler;

};

class Config {

public:

    Config():path("config.yaml"){}

    void SetPath(const std::string &path) {
        this->path = std::string(path);
        Logger::logDebug("Configuration file path set to %s") << path;
    }

    template <typename T>
    void Set(const ConfigKey<T> &key, T value) {
        config[std::string(key.Parent)][std::string(key.Key)] = value;
    }

    template <typename T>
    T Get(const ConfigKey<T> &key) {

        if(key.Handler) {
            return key.Handler(this);
        }

        return config[std::string(key.Parent)][std::string(key.Key)].as<T>();
    }

    void Load() {

        Logger::logDebug("Loading configurations from path %s") << path;

        std::ifstream file(path);

        if(!file.good()) {
            Logger::logDebug("Configuration file not found at %s. A new one will be created with default settings") << path;
            ConfigDefaults::SetDefaults(this);
            Save();
        }

        config = YAML::LoadFile(path);
    }

    void Save() {
        Logger::logDebug("Saving configuration file at %s") << path;
        std::ofstream file(path);
        file << config;
        file.close();
    }

private:
    YAML::Node config;
    std::string path;
};


#endif //PROY_GRADO_CONFIG_H
