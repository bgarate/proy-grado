//
// Created by bruno on 08/08/17.
//

#include "World.h"

void World::addMarker(cv::Vec3d position, cv::Vec3d rotation, int id) {
    std::unique_lock<std::mutex> lck(objectsMutex);
    objects.push_back(new WorldObject(position, rotation, ObjectType::MARKER, id));
}

void World::addDrone(cv::Vec3d position, cv::Vec3d rotation, int id) {
    std::unique_lock<std::mutex> lck(objectsMutex);
    objects.push_back(new WorldObject(position, rotation, ObjectType::DRONE, id));
}

std::vector<WorldObject *> World::getObjects() {
    std::unique_lock<std::mutex> lck(objectsMutex);
    return std::vector<WorldObject *>(objects.begin(), objects.end());
}

std::vector<WorldObject *> World::getDrones() {
    std::unique_lock<std::mutex> lck(objectsMutex);
    std::vector<WorldObject *> ret;

    std::copy_if(objects.begin(), objects.end(),
                 std::back_inserter(ret), [](const WorldObject* o){return o->getType() == ObjectType::DRONE;} );

    return ret;
}

std::vector<WorldObject *> World::getMarkers() {
    std::unique_lock<std::mutex> lck(objectsMutex);
    std::vector<WorldObject *> ret;

    std::copy_if(objects.begin(), objects.end(),
                 std::back_inserter(ret), [](const WorldObject* o){return o->getType() == ObjectType::MARKER;} );

    return ret;
}

const cv::Vec3d WorldObject::getPosition() {
    std::unique_lock<std::mutex> lck(positionMutex);
    return cv::Vec3d(position);
}

const cv::Vec3d WorldObject::getRotation() {
    std::unique_lock<std::mutex> lck(rotationMutex);
    return cv::Vec3d(rotation);
}

void WorldObject::setPosition(const cv::Vec3d &position) {
    std::unique_lock<std::mutex> lck(positionMutex);
    this->position = cv::Vec3d(position);
}

void WorldObject::setRotation(const cv::Vec3d &rotation) {
    std::unique_lock<std::mutex> lck(rotationMutex);
    this->rotation = cv::Vec3d(rotation);
}

int WorldObject::getId() const {
    return id;
}

ObjectType WorldObject::getType() const {
    return type;
}
