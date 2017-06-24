//
// Created by bruno on 01/04/17.
//

#include <src/logging/Logger.h>
#include "MultiTracker.h"

void MultiTracker::setTargets(std::vector<cv::Rect2d> targets, std::shared_ptr<cv::Mat> frame) {
    trackers = cv::MultiTracker(getAlgorithm(algorithm));
    trackers.add(*frame, targets);
}

std::vector<cv::Rect2d> MultiTracker::getTargets() {
    return trackers.objects;
}

bool MultiTracker::track(std::shared_ptr<cv::Mat> track) {

    std::vector<cv::Rect2d> tracks;

    bool ret = trackers.update(*track);

    for (cv::Rect t : getTargets())
    {
        if(t.x + t.width > 0 && t.y + t.height > 0 && t.x < track->cols && t.y < track->rows)
            tracks.push_back(t);
    }

    if(tracks.size() != getTargets().size()) {
        Logger::logDebug("Track lost");
        setTargets(tracks, track);
    }

    return ret;
}

MultiTracker::MultiTracker(Algorithm algorithm): trackers(getAlgorithm(algorithm)) {
    this->algorithm = algorithm;
}

std::string MultiTracker::getAlgorithm(Algorithm algorithm){

    switch (algorithm){
        case Algorithm::KCF:
            return "KCF";
        case Algorithm::MEDIANFLOW:
            return "MEDIANFLOW";
        case Algorithm::MIL:
            return "MIL";
        case Algorithm::BOOSTING:
            return "BOOSTING";
        case Algorithm::TLD:
            return "TLD";
    }
}
