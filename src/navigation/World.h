//
// Created by bruno on 08/08/17.
//

#ifndef PROY_GRADO_WORLD_H
#define PROY_GRADO_WORLD_H

#include <mutex>
#include "opencv2/opencv.hpp"

enum class ObjectType {
    DRONE, MARKER, PAD
};

class WorldObject {
public:
    WorldObject() {
        cameraMatrix = (cv::Mat)cv::Matx44d();
    }

    WorldObject(cv::Vec3d position, cv::Vec3d rotation, ObjectType type, int id, std::string state):
            position(position), rotation(rotation), type(type), id(id), state(state) {
        cameraMatrix = (cv::Mat)cv::Matx44d();

    }

    const cv::Vec3d getPosition();
    const cv::Vec3d getRotation();
    void setInversePose(const cv::Vec3d& position, const cv::Vec3d& rotation);
    int getId() const;
    ObjectType getType() const;
    std::string getState() const;
    cv::Mat getCameraMatrix();

    void setRotation(const cv::Vec3d rotation);

    void setPosition(const cv::Vec3d position);

    void setState(const std::string state);

private:

    std::string state;
    cv::Vec3d position;
    cv::Vec3d rotation;
    cv::Mat cameraMatrix;
    ObjectType type;
    int id;
    std::mutex mutex;
    void calculateObjectMatrix(const cv::Vec3d &position, const cv::Vec3d &rotation);

};

class World {

public:
    World();
    World(World&& other);
    World(World &other);
    World& operator=(const World& other);
    World& operator=(World &&other);

    void addMarker(cv::Vec3d position, cv::Vec3d rotation, int id, std::string state);
    void addDrone(cv::Vec3d position, cv::Vec3d rotation, int id, std::string state);
    void addObject(ObjectType type, cv::Vec3d position, cv::Vec3d rotation, int id, std::string state);
    void addPad(cv::Vec3d position, cv::Vec3d rotation, int id);

    std::vector<WorldObject*> getObjects();
    std::vector<WorldObject*> getDrones();
    WorldObject* getDrone(int id);
    std::vector<WorldObject*> getMarkers();
    std::vector<WorldObject*> getPads();
    WorldObject *getPad(int id);

    WorldObject* getMarker(int id);
private:
    std::vector<WorldObject*> objects;
    mutable std::mutex objectsMutex;

};


#endif //PROY_GRADO_WORLD_H
