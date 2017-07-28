#include <opencv2/opencv.hpp>

#ifndef PROY_GRADO_MARKERLAND_H
#define PROY_GRADO_MARKERLAND_H

enum class LandingState {Inactive ,Finding, Rotating, Centring, FinalPositioning};

class MarkerLand {

public:
    MarkerLand();

    void land(std::vector<cv::Point> points, cv::Point frameSize);

    bool isLanding();

    void stopLanding();

private:
    LandingState state;

    float alignmentTolerance = 0.1;

};


#endif //PROY_GRADO_MARKERLAND_H
