#include <opencv2/opencv.hpp>
#include <src/config/Config.h>

#ifndef PROY_GRADO_MARKERLAND_H
#define PROY_GRADO_MARKERLAND_H

enum class LandingState {Inactive, Centring, Landing, Lost};

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

    long lastReferenceTime;
    int moveWithoutReferenceTime;

    float withoutReferenceGaz;

    long firstAlignmentTime;
    int stabilisationTime;

    long deltaTime = 0;
    long runningTime = 0;
    std::chrono::steady_clock::time_point startTime;
    std::chrono::steady_clock::time_point lastTime;
    std::chrono::steady_clock::time_point newTime;

    bool preland;

    LandMoveCommand lastres;

};


#endif //PROY_GRADO_MARKERLAND_H
