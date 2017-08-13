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
        cameraMatrix = (cv::Mat)cv::Matx44d();
    }

    WorldObject(cv::Vec3d position, cv::Vec3d rotation, ObjectType type, int id):
            position(position), rotation(rotation), type(type), id(id) {
        cameraMatrix = (cv::Mat)cv::Matx44d();

    }

    const cv::Vec3d getPosition();
    const cv::Vec3d getRotation();
    void setInversePose(const cv::Vec3d& position, const cv::Vec3d& rotation);
    int getId() const;
    ObjectType getType() const;
    cv::Mat getCameraMatrix();

    void setRotation(const cv::Vec3d rotation);

    void setPosition(const cv::Vec3d position);

private:

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
    void addMarker(cv::Vec3d position, cv::Vec3d rotation, int id);
    void addDrone(cv::Vec3d position, cv::Vec3d rotation, int id);
    std::vector<WorldObject*> getObjects();
    std::vector<WorldObject*> getDrones();
    std::vector<WorldObject*> getMarkers();

    WorldObject* getMarker(int id);

private:
    std::vector<WorldObject*> objects;
    std::mutex objectsMutex;

};


#endif //PROY_GRADO_WORLD_H
