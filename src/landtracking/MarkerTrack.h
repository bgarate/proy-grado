#include <opencv2/opencv.hpp>
#include <boost/shared_ptr.hpp>

#ifndef PROY_GRADO_MARKTRACK_H
#define PROY_GRADO_MARKTRACK_H

class MarkerTrack {

private:

    float const squareTolerance = 0.2;

public:

    MarkerTrack();

    std::vector<cv::Point> Track(std::shared_ptr<cv::Mat> frame);

};


#endif
