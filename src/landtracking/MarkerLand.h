#include <opencv2/opencv.hpp>
#include <src/config/Config.h>

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

    explicit MarkerLand(Config* config);

    LandMoveCommand land(std::vector<cv::Point> points, cv::Point frameSize, double altitude);

    bool isLanding();

    void stopLanding();

private:
    LandingState state;

    float pitchvelfactor;
    float rollvelfactor;
    const float yawvelfactor = 0.6;
    float gazvelfactor;

    float landAltitude;

    float pitchtolerance;
    float rolltolerance;
    const float yawtolerance = 0.05;

    float gazpreland;
    float pitchpreland;
    float rollpreland;

    const int maxSetpsWithoutReference = 100;
    const float withoutReferenceGaz = 0.05;

    const int contZeroVelTolerance = 5;

    int countWithoutReference;
    int countZeroVel;
    bool preland;

    LandMoveCommand lastres;

};


#endif //PROY_GRADO_MARKERLAND_H
