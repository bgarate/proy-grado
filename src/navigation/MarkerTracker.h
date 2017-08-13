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

        Distance = (float)cv::norm(Translation);

    }

    cv::Point2i getCenter() const{
        return cv::Point2i(
                (int)((Corners[0].x + Corners[2].x)/2),
                (int)((Corners[0].y + Corners[2].y)/2));
    }

    cv::Vec3d getXYZPosition() const{
        return cv::Vec3d(Translation[0],Translation[2],-Translation[1]);
    }

    cv::Vec3d getEulerAngles() const{

        cv::Mat rotationMatrix;
        cv::Rodrigues(Rotation,rotationMatrix);

        cv::Mat cameraMatrix,rotMatrix,transVect,rotMatrixX,rotMatrixY,rotMatrixZ;

        double* _r = rotationMatrix.ptr<double>();
        double projMatrix[12] = {_r[0],_r[1],_r[2],0,
                                 _r[3],_r[4],_r[5],0,
                                 _r[6],_r[7],_r[8],0};

        cv::Vec3d eulerAngles;
        decomposeProjectionMatrix(cv::Mat(3,4,CV_64FC1,projMatrix),
                                  cameraMatrix,
                                  rotMatrix,
                                  transVect,
                                  rotMatrixX,
                                  rotMatrixY,
                                  rotMatrixZ,
                                  eulerAngles);

        return eulerAngles;
    }

    // Asumo que los marcadores siempre deberian estar por debajo de la linea de la camara
    bool isFacingUp() const{
        cv::Mat rotationMatrix;
        cv::Rodrigues(Rotation,rotationMatrix);
        rotationMatrix = rotationMatrix.t();
        cv::Mat inverseTranslation = -rotationMatrix * (cv::Mat)Translation;
        return Translation[1] > 0;
    }

    std::vector<cv::Point2f> Corners;
    int Id;
    cv::Vec3d Rotation;
    cv::Vec3d Translation;
    float Distance;
};

class MarkerTracker {
public:
    MarkerTracker(Config* config);

    std::vector<Marker> Markers;
    cv::Mat getImage(int code);
    int getDictionarySize();
    void Update(std::shared_ptr<cv::Mat> frame, double deltaTime);

    static constexpr float MARKER_SIDE_LENGTH = 0.12;

private:

    int const DICTIONARY_TO_USE = cv::aruco::DICT_4X4_50;
    int const MARKER_SIZE = 72*18;
    int const BORDER_SIZE = 1;
    cv::Ptr<cv::aruco::Dictionary> dictionary;
    std::vector<cv::Vec3d> translations;
    std::vector<cv::Vec3d> rotations;

    Config *config;
};


#endif //PROY_GRADO_MARKERTRACKER_H
