//
// Created by bruno on 01/04/17.
//

#ifndef PROY_GRADO_MULTITRACKER_H
#define PROY_GRADO_MULTITRACKER_H


#include <memory>
#include "TrackingAlgorithm.h"

class MultiTracker : public TrackingAlgorithm {
public:
    enum class Algorithm {MEDIANFLOW, MIL, BOOSTING, KCF, TLD};
    MultiTracker(Algorithm algorithm);

    void setTargets(std::vector<cv::Rect2d> targets, std::shared_ptr<cv::Mat> frame) override;
    std::vector<cv::Rect2d> getTargets() override;
    bool track(std::shared_ptr<cv::Mat> track) override;
private:
    std::vector<cv::Rect2d> targets;
    cv::MultiTracker trackers;
    Algorithm  algorithm;
    std::string getAlgorithm(Algorithm algorithm);
};


#endif //PROY_GRADO_MULTITRACKER_H
