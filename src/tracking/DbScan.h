//
// Created by bruno on 24/07/17.
//

#ifndef PROY_GRADO_DBSCAN_H
#define PROY_GRADO_DBSCAN_H


#include <opencv2/core/types.hpp>

typedef struct {
    bool visited = false;
    bool inCluster = false;
} PointStatus;

class DbScan {
public:
    DbScan(std::vector<cv::Point2f>* points, double epsilon, int minPoints);

    std::vector<std::vector<cv::Point2f>> run();
private:
    std::vector<cv::Point2f>* points;
    std::vector<PointStatus> status;
    double squaredEpsilon;
    int minPoints;

    std::vector<int> getNeighborhood(const cv::Point2f& point);

    std::vector<cv::Point2f> createCluster(std::vector<int> neighbors);
};


#endif //PROY_GRADO_DBSCAN_H
