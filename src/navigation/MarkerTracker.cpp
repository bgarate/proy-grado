//
// Created by bruno on 30/07/17.
//

#include <memory>
#include <src/Config.h>
#include <thread>
#include <src/logging/Logger.h>
#include <src/ConfigKeys.h>
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

    std::vector<int> ids;

    cv::aruco::detectMarkers(*frame, dictionary,
        corners, ids);

    cv::aruco::estimatePoseSingleMarkers(corners,MARKER_SIDE_LENGTH,
                                         config->Get(ConfigKeys::Drone::CameraMatrix),
                                         config->Get(ConfigKeys::Drone::DistortionCoefficients),
                                         rotations, translations);

    Markers.clear();

    for (int i = 0; i < corners.size(); ++i) {

        Marker marker(ids[i], corners[i], rotations[i], translations[i]);

        if(!marker.isFacingUp()) {
            Logger::logWarning("Marker %u discarded as it was incorrectly positioned: %s %s")
                    << marker.Id << marker.getXYZPosition() << marker.getEulerAngles();
        }

        Markers.push_back(marker);
    }

}


