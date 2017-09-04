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

    const float pitchvelfactor = 0.06;
    const float rollvelfactor = 0.08;
    const float yawvelfactor = 0.6;
    const float gazvelfactor = 0.5;

    const float landAltitude = 1.0;

    const float pitchtolerance = 0.01;
    const float rolltolerance = 0.005;
    const float yawtolerance = 0.05;

    const float gazpreland = -0.5;
    const float pitchpreland = 0.7;
    const float rollpreland = -0.4;

    bool preland = false;

    LandMoveCommand lastres;

};


#endif //PROY_GRADO_MARKERLAND_H
