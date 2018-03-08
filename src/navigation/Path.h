//
// Created by bruno on 14/10/17.
//

#ifndef PROY_GRADO_PATH_H
#define PROY_GRADO_PATH_H


#include <opencv2/opencv.hpp>

class PathPoint {
public:
    PathPoint() {

    }
    PathPoint(cv::Vec3d position, double rotation) : position(position), rotation(rotation) {

    }
    cv::Vec3d position;
    double rotation;
};

class Path {
public:
    Path();
    void AddPoint(cv::Vec3d position, double rotation);
    std::vector<PathPoint> GetPoints();
private:
    std::vector<PathPoint> points;
};


#endif //PROY_GRADO_PATH_H
