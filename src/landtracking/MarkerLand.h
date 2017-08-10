#include <opencv2/opencv.hpp>

#ifndef PROY_GRADO_MARKERLAND_H
#define PROY_GRADO_MARKERLAND_H

enum class LandingState {Inactive, Centring, Landing};

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

    const float pitchvelfactor = 0.12;
    const float rollvelfactor = 0.16;
    const float yawvelfactor = 1;
    const float gazvelfactor = 0.5;

    const float landAltitude = 1.5;

    const float xtolerance=0.025, ytolerance=0.05, altitudetolereance=1;

};


#endif //PROY_GRADO_MARKERLAND_H
