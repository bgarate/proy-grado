//
// Created by bruno on 01/04/17.
//

#include "MultiTracker.h"

void MultiTracker::setTargets(std::vector<cv::Rect2d> targets, cv::Mat frame) {
    trackers = cv::MultiTracker(getAlgorithm(algorithm));
    trackers.add(frame, targets);
}

std::vector<cv::Rect2d> MultiTracker::getTargets() {
    return trackers.objects;
}

bool MultiTracker::track(cv::Mat track) {
    return trackers.update(track);
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
