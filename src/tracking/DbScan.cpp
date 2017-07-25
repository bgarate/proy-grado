//
// Created by bruno on 24/07/17.
//

#include "DbScan.h"




std::vector<std::vector<cv::Point2f>> DbScan::run() {

    std::vector<std::vector<cv::Point2f>> clusters;

    for (int i = 0; i < points->size(); ++i) {

        if(status[i].visited)
            continue;

        cv::Point2f p = (*points)[i];

        status[i].visited = true;
        std::vector<int> neighbors = getNeighborhood(p);

        if(neighbors.size() >= minPoints) {
            std::vector<cv::Point2f> currentCluster = createCluster(neighbors);
            currentCluster.push_back(p);
            clusters.push_back(currentCluster);
        }

    }

    return clusters;

}

std::vector<int> DbScan::getNeighborhood(const cv::Point2f& point) {

    std::vector<int> neighbors;

    for(int i = 0; i < points->size(); i++) {
        cv::Point2f p = points->at(i);
        cv::Point2f v = point - p;
        float distance = v.x * v.x + v.y * v.y;
        if (distance < squaredEpsilon && !(point.x == p.x && point.y == p.y))
            neighbors.push_back(i);
    }

    return neighbors;

};

DbScan::DbScan(std::vector<cv::Point2f>* points, double epsilon, int minPoints)
:squaredEpsilon(epsilon*epsilon), minPoints(minPoints), points(points), status(points->size(), PointStatus()) {

}

std::vector<cv::Point2f> DbScan::createCluster(std::vector<int> neighbors) {

    std::vector<cv::Point2f> cluster;

    int i = 0;
    while(i < neighbors.size()) {
        cv::Point2f p = points->at(neighbors[i]);

        if(!status[neighbors[i]].visited) {
            status[neighbors[i]].visited = true;
            std::vector<int> newNeighbors = getNeighborhood(p);

            if(newNeighbors.size() >= minPoints)
                neighbors.insert(neighbors.end(),newNeighbors.begin(), newNeighbors.end());

        }

        if(!status[neighbors[i]].inCluster) {
            cluster.push_back(p);
            status[neighbors[i]].inCluster = true;
        }

        i++;
    }

    return cluster;

}
