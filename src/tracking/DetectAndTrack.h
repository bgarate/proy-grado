//
// Created by bruno on 01/04/17.
//

#ifndef PROY_GRADO_DETECTANDTRACK_H
#define PROY_GRADO_DETECTANDTRACK_H


#include "DetectionAlgorithm.h"
#include "TrackingAlgorithm.h"
#include "Track.h"

class DetectAndTrack {
public:
    DetectAndTrack(DetectionAlgorithm* detector, TrackingAlgorithm* tracker);
    std::vector<Track> update(cv::Mat frame);
    const double KEEP_TRACK_OVERLAP_THRESHOLD = 0.3;
private:
    DetectionAlgorithm* detector;
    TrackingAlgorithm* tracker;
    std::vector<Track> tracks;
    std::vector<Track> updateDetections(std::vector<cv::Rect2d> newDectections);
    void updateTracks(std::vector<cv::Rect2d> tracking);
    int trackCount = 0;
    int trackedFrames = 0;
};


#endif //PROY_GRADO_DETECTANDTRACK_H
