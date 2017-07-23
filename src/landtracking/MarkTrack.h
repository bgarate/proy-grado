#include <opencv2/opencv.hpp>
#include <boost/shared_ptr.hpp>

#ifndef PROY_GRADO_MARKTRACK_H
#define PROY_GRADO_MARKTRACK_H

class MarkTrack {

private:

    bool inicialized;
    std::vector<std::vector<cv::Point>> contours;
    //std::vector<std::vector<cv::Point>> squareContours;
    std::vector<cv::Vec4i> hierarchy;
    std::vector<cv::Point> pointsseq;
    int mark;
    int marklist[4];

    float const squareTolerance = 0.15;

    cv::Mat *gray, *edges;

public:

    MarkTrack();

    std::vector<std::vector<cv::Point>> Track(std::shared_ptr<cv::Mat> frame);

};


#endif
