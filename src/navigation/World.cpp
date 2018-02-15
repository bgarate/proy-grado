//
// Created by bruno on 08/08/17.
//

#include <opencv/cv.hpp>
#include <iostream>
#include "World.h"

void World::addMarker(cv::Vec3d position, cv::Vec3d rotation, int id) {
    addObject(ObjectType::MARKER, position, rotation, id);
}

void World::addDrone(cv::Vec3d position, cv::Vec3d rotation, int id) {
    addObject(ObjectType::DRONE, position, rotation, id);
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

WorldObject* World::getMarker(int id) {
    std::unique_lock<std::mutex> lck(objectsMutex);

    auto it = std::find_if(objects.begin(), objects.end(),
                           [id](const WorldObject* o){return o->getId() == id &&
                                   o->getType() == ObjectType::MARKER;} );

    return it != objects.end() ? *it : NULL;
}

std::vector<WorldObject *> World::getMarkers() {
    std::unique_lock<std::mutex> lck(objectsMutex);
    std::vector<WorldObject *> ret;

    std::copy_if(objects.begin(), objects.end(),
                 std::back_inserter(ret), [](const WorldObject* o){return o->getType() == ObjectType::MARKER;} );

    return ret;
}

std::vector<WorldObject *> World::getPads() {
    std::unique_lock<std::mutex> lck(objectsMutex);
    std::vector<WorldObject *> ret;

    std::copy_if(objects.begin(), objects.end(),
                 std::back_inserter(ret), [](const WorldObject* o){return o->getType() == ObjectType::PAD;} );

    return ret;
}

void World::addObject(ObjectType type, cv::Vec3d position, cv::Vec3d rotation, int id) {
    std::unique_lock<std::mutex> lck(objectsMutex);
    objects.push_back(new WorldObject(position, rotation, type, id));
}

World::World(World&& other) {
    std::lock_guard<std::mutex> lock(other.objectsMutex);
    objects = std::move(other.objects);
}

World::World(World& other) {
    std::lock_guard<std::mutex> lock(other.objectsMutex);
    objects = other.objects;
}

World::World() {

}

World &World::operator=(const World &other) {
    if(this !=  &other) {
        std::unique_lock<std::mutex> otherLock(other.objectsMutex, std::defer_lock);
        std::unique_lock<std::mutex> lock(objectsMutex, std::defer_lock);
        std::lock(otherLock, lock);
        objects = other.objects;
    }
    return *this;
}

World& World::operator=(World &&other) {
    if(this !=  &other) {
        std::unique_lock<std::mutex> otherLock(other.objectsMutex, std::defer_lock);
        std::unique_lock<std::mutex> lock(objectsMutex, std::defer_lock);
        std::lock(otherLock, lock);
        objects = other.objects;
    }
    return *this;
}

void WorldObject::setPosition(const cv::Vec3d position) {
    std::unique_lock<std::mutex> lck(mutex);
    this->position = position;
}

void WorldObject::setRotation(const cv::Vec3d rotation) {
    std::unique_lock<std::mutex> lck(mutex);
    this->rotation = rotation;
}

const cv::Vec3d WorldObject::getPosition() {
    std::unique_lock<std::mutex> lck(mutex);
    return cv::Vec3d(position);
}

const cv::Vec3d WorldObject::getRotation() {
    std::unique_lock<std::mutex> lck(mutex);
    return cv::Vec3d(rotation);
}

void WorldObject::setInversePose(const cv::Vec3d &position, const cv::Vec3d &rotation) {
    std::unique_lock<std::mutex> lck(mutex);
    calculateObjectMatrix(position, rotation);
}

int WorldObject::getId() const {
    return id;
}

ObjectType WorldObject::getType() const {
    return type;
}

void WorldObject::calculateObjectMatrix(const cv::Vec3d &position, const cv::Vec3d &rotation) {
    // Calc camera pose
    cv::Mat R;
    cv::Rodrigues(rotation, R);


    R = R.t();
    cv::Mat cameraPose = -R * (cv::Mat)position;

    cv::Vec3d inverseRotation;
    cv::Rodrigues(R, inverseRotation);

    this->position = cv::Vec3d(cameraPose);
    this->rotation = inverseRotation;


    cameraMatrix = cv::Mat::eye(4, 4, R.type());
    R.copyTo(cameraMatrix.rowRange(0, 3).colRange(0, 3)); // copies R into camPose
    cameraPose.copyTo(cameraMatrix.rowRange(0, 3).colRange(3, 4)); // copies tvec into camPose
    double* x = &cameraMatrix.at<double>(3,3);

    *x = 1;


}

cv::Mat WorldObject::getCameraMatrix() {
    std::unique_lock<std::mutex> lck(mutex);
    cv::Mat ret;
    cameraMatrix.copyTo(ret);
    return ret;
}
