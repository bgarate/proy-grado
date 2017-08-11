//
// Created by bruno on 30/07/17.
//

#include <memory>
#include <src/Config.h>
#include <thread>
#include "MarkerTracker.h"

MarkerTracker::MarkerTracker(Config* config) {
    dictionary = cv::aruco::getPredefinedDictionary(DICTIONARY_TO_USE);
    this->config = config;
}

cv::Mat MarkerTracker::getImage(int code) {
    cv::Mat img;
    dictionary->drawMarker(code, MARKER_SIZE, img, BORDER_SIZE);
    return img;
}

int MarkerTracker::getDictionarySize() {
    return dictionary->bytesList.rows;
}

void MarkerTracker::Update(std::shared_ptr<cv::Mat> frame, double deltaTime) {

    std::vector<std::vector<cv::Point2f>> corners;
    std::vector<cv::Vec3d> translations;
    std::vector<cv::Vec3d> rotations;
    std::vector<int> ids;

    cv::aruco::detectMarkers(*frame, dictionary,
        corners, ids);

    cv::aruco::estimatePoseSingleMarkers(corners,MARKER_SIDE_LENGTH,
                                         config->getCameraMatrix(),
                                         config->getDistortionCoefficients(),
                                         rotations, translations);

    Markers.clear();

    for (int i = 0; i < corners.size(); ++i) {
        Markers.push_back(Marker(ids[i], corners[i], rotations[i], translations[i]));
    }

}


