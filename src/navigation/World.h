//
// Created by bruno on 08/08/17.
//

#ifndef PROY_GRADO_WORLD_H
#define PROY_GRADO_WORLD_H

#include <opencv2/core/matx.hpp>
#include <mutex>

enum class ObjectType {
    DRONE, MARKER
};

class WorldObject {
public:
    WorldObject() {

    }

    WorldObject(cv::Vec3d position, cv::Vec3d rotation, ObjectType type, int id):
            position(position), rotation(rotation), type(type), id(id) {

    }

    const cv::Vec3d getPosition();
    const cv::Vec3d getRotation();
    void setPosition(const cv::Vec3d& position);
    void setRotation(const cv::Vec3d& rotation);
    int getId() const;
    ObjectType getType() const;

private:

    cv::Vec3d position;
    cv::Vec3d rotation;
    ObjectType type;
    int id;
    std::mutex positionMutex;
    std::mutex rotationMutex;
};

class World {

public:
    void addMarker(cv::Vec3d position, cv::Vec3d rotation, int id);
    void addDrone(cv::Vec3d position, cv::Vec3d rotation, int id);
    std::vector<WorldObject*> getObjects();
    std::vector<WorldObject*> getDrones();
    std::vector<WorldObject*> getMarkers();
private:
    std::vector<WorldObject*> objects;
    std::mutex objectsMutex;
};


#endif //PROY_GRADO_WORLD_H
