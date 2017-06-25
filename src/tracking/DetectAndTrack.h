//
// Created by bruno on 01/04/17.
//

#ifndef PROY_GRADO_DETECTANDTRACK_H
#define PROY_GRADO_DETECTANDTRACK_H

#include "memory"

#include "DetectionAlgorithm.h"
#include "TrackingAlgorithm.h"
#include "Track.h"

class DetectAndTrack {
public:
    DetectAndTrack(DetectionAlgorithm* detector, TrackingAlgorithm* tracker);
    std::vector<Track> update(std::shared_ptr<cv::Mat> frame);
    const double KEEP_TRACK_OVERLAP_THRESHOLD = 0.3;
    const double FRAMES_TO_DETECT = 2;
    static constexpr const double ROI_MARGIN = 0.2;
private:
    DetectionAlgorithm* detector;
    TrackingAlgorithm* tracker;
    std::vector<Track> tracks;
    std::vector<cv::Rect2d> accumulatedFound;
    std::vector<Track> updateDetections(std::vector<cv::Rect2d> newDectections);

    int trackCount = 0;
    int trackedFrames = 0;

    void filter_rects(std::vector<cv::Rect2d> &candidates, unsigned long tracksKept, std::vector<cv::Rect2d> &objects);

    bool insideROI(cv::Rect2d r,std::shared_ptr<cv::Mat> frame);

    void updateTracks(std::vector<cv::Rect2d> tracking, std::shared_ptr<cv::Mat> frame);
};


#endif //PROY_GRADO_DETECTANDTRACK_H
