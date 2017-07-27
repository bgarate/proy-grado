#include <opencv2/opencv.hpp>

#ifndef PROY_GRADO_MARKERLAND_H
#define PROY_GRADO_MARKERLAND_H


class MarkerLand {

public:
    MarkerLand();

    void land(cv::Point);

    bool isLanding();

    void stopLanding();

};


#endif //PROY_GRADO_MARKERLAND_H
