#include <opencv2/opencv.hpp>

#ifndef PROY_GRADO_MARKERLAND_H
#define PROY_GRADO_MARKERLAND_H

enum class LandingState {Inactive ,Finding, Rotating, Centring, FinalPositioning};

class LandMoveCommand {
public:
    bool land;
    float pitch;
    float roll;
    float yaw;
    float gaz;
    LandingState state;
};

class MarkerLand {

public:
    MarkerLand();

    LandMoveCommand land(std::vector<cv::Point> points, cv::Point frameSize, double altitude);

    bool isLanding();

    void stopLanding();

private:
    LandingState state;

    const float alignmentTolerance = 0.05;
    const float landAltitude = 2;
    const float pitchPorcent = 0.05;
    const float rollPorcent = 0.05;
    const float yawPorcent = 0.1;
    const float gazPorcent = 0.3;

};


#endif //PROY_GRADO_MARKERLAND_H
