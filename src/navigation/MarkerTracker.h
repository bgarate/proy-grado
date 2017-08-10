//
// Created by bruno on 30/07/17.
//

#ifndef PROY_GRADO_MARKERTRACKER_H
#define PROY_GRADO_MARKERTRACKER_H

#include "src/Config.h"
#include "opencv2/aruco.hpp"
#include "memory"

struct Marker {

    Marker() {

    }

    Marker(int id, std::vector<cv::Point2f> corners,
        cv::Vec3d rotation, cv::Vec3d translation) :
            Id(id), Corners(corners), Rotation(rotation),
            Translation(translation) {

        // Calc camera pose
        cv::Mat R;
        cv::Rodrigues(rotation, R);
        EstimatedCameraPose = -R.t() * (cv::Mat)translation;
    }

    std::vector<cv::Point2f> Corners;
    int Id;
    cv::Vec3d Rotation;
    cv::Vec3d Translation;
    cv::Mat EstimatedCameraPose;
    float Distance;
};

class MarkerTracker {
public:
    MarkerTracker(Config* config);

    std::vector<Marker> Markers;
    cv::Mat getImage(int code);
    int getDictionarySize();
    void Update(std::shared_ptr<cv::Mat> frame, double deltaTime);

    static constexpr float MARKER_SIDE_LENGTH = 0.089;

private:

    int const DICTIONARY_TO_USE = cv::aruco::DICT_4X4_50;
    int const MARKER_SIZE = 72*18;
    int const BORDER_SIZE = 1;
    cv::Ptr<cv::aruco::Dictionary> dictionary;

    Config *config;
};


#endif //PROY_GRADO_MARKERTRACKER_H
