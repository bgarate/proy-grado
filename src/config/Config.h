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
#include "../logging/Logger.h"



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
    std::shared_ptr<std::mutex> mutex = std::shared_ptr<std::mutex>(new std::mutex);

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
        std::lock_guard<std::mutex> lck(*key.mutex);

        config[std::string(key.Parent)][std::string(key.Key)] = value;
    }

    template <typename T>
    T Get(const ConfigKey<T> &key) {
        std::lock_guard<std::mutex> lck(*key.mutex);

        if(key.Handler) {
            return key.Handler(this);
        }

        return config[std::string(key.Parent)][std::string(key.Key)].as<T>();
    }

    ObjectType GetObjectTypeFromString(std::string str) {
        if(str == "marker")
            return ObjectType::MARKER;
        if(str == "drone")
            return ObjectType::DRONE;
        if(str == "pad")
            return ObjectType::PAD;

        throw new std::runtime_error("Uknown object type " + str);
    }

    std::string GetStringFromObjectType(ObjectType type) {
        switch (type) {
            case ObjectType::DRONE:
                return "drone";
            case ObjectType::MARKER:
                return "marker";
            case ObjectType::PAD:
                return "pad";
            default:
                throw new std::runtime_error("Uknown object type");
        }
    }

    World GetWorld() {
        std::lock_guard<std::mutex> lck(worldMutex);

        YAML::Node worldNode = config["World"];
        YAML::Node objects = worldNode["Objects"];

        World world;

        for (int i = 0; i < objects.size(); ++i) {

            YAML::Node object = objects[i];
            ObjectType type = GetObjectTypeFromString(object["type"].as<std::string>());
            cv::Vec3d position = object["position"].as<cv::Vec3d>();
            cv::Vec3d rotation = object["rotation"].as<cv::Vec3d>();
            int id = object["id"].as<int>();

            world.addObject(type, position, rotation, id, "");

        }

        return world;

    }

    void SetWorld(World world) {
        std::lock_guard<std::mutex> lck(worldMutex);

        YAML::Node worldNode = config["World"];
        YAML::Node objects = worldNode["Objects"];

        std::vector<WorldObject *> worldObjects = world.getObjects();

        for (int i = 0; i < worldObjects.size(); ++i) {

            YAML::Node object;
            WorldObject *worldObject = worldObjects[i];

            object["id"] = worldObject->getId();
            object["type"] = GetStringFromObjectType(worldObject->getType());
            object["position"] = worldObject->getPosition();
            object["rotation"] = worldObject->getRotation();

            objects.push_back(object);

        }

    }

    Path GetPath() {
        std::lock_guard<std::mutex> lck(pathMutex);

        YAML::Node pathNode = config["Path"];

        Path path;

        for (int i = 0; i < pathNode.size(); ++i) {

            YAML::Node object = pathNode[i];
            cv::Vec3d position = object["position"].as<cv::Vec3d>();
            double rotation = object["rotation"].as<double>();

            path.AddPoint(position, rotation);

        }

        return path;

    }

    void SetPath(Path path) {
        std::lock_guard<std::mutex> lck(pathMutex);

        YAML::Node pathNode = config["Path"];

        std::vector<PathPoint> points = path.GetPoints();

        for (int i = 0; i < points.size(); ++i) {

            YAML::Node object;
            PathPoint point = points[i];

            object["position"] = point.Postion;
            object["rotation"] = point.Rotation;

            pathNode.push_back(object);
        }

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

    std::mutex worldMutex;
    std::mutex pathMutex;

};


#endif //PROY_GRADO_CONFIG_H
