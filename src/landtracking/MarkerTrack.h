#include <opencv2/opencv.hpp>
#include <boost/shared_ptr.hpp>
#include <src/config/Config.h>

#ifndef PROY_GRADO_MARKTRACK_H
#define PROY_GRADO_MARKTRACK_H

class MarkerTrack {

private:

    float squareTolerance;

public:

    std::vector<std::vector<cv::Point>> redContours;
    std::vector<cv::Rect> redRects;
    MarkerTrack(Config* config);

    std::vector<cv::Point> Track(std::shared_ptr<cv::Mat> frame);
    void CalculateRedZones(cv::Mat frame);

};


#endif
