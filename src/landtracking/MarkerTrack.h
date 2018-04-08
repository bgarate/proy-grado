#include <opencv2/opencv.hpp>
#include <boost/shared_ptr.hpp>

#ifndef PROY_GRADO_MARKTRACK_H
#define PROY_GRADO_MARKTRACK_H

class MarkerTrack {

private:

    float const squareTolerance = 0.2;


public:

    std::vector<std::vector<cv::Point>> redContours;
    std::vector<cv::Rect> redRects;
    MarkerTrack();

    std::vector<cv::Point> Track(std::shared_ptr<cv::Mat> frame);
    void CalculateRedZones(cv::Mat frame);

};


#endif
