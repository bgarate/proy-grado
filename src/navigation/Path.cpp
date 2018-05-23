//
// Created by bruno on 14/10/17.
//

#include "Path.h"

Path::Path() {

}

void Path::AddPoint(cv::Vec3d position, double rotation) {
    points.push_back(PathPoint(position, rotation));
}

std::vector<PathPoint> Path::GetPoints() {
    return std::vector<PathPoint>(points);
}
