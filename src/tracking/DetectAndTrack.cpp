//
// Created by bruno on 01/04/17.
//

#include "DetectAndTrack.h"
#include "../logging/Logger.h"

DetectAndTrack::DetectAndTrack(DetectionAlgorithm *detector, TrackingAlgorithm *tracker) {
    this->detector = detector;
    this->tracker = tracker;
}

std::vector<Track> DetectAndTrack::update(cv::Mat frame) {

    if(trackedFrames % 50 == 0) {

        detector->detect(frame);

        if(detector->getFound().empty())
            return {};

        tracker->setTargets(detector->getFound(), frame);

        int oldTrackingCount = trackCount;
        tracks = updateDetections(detector->getFound());

        Logger::logDebug("%u objects detected. %u new tracks") <<
                         detector->getFound().size() << trackCount - oldTrackingCount;

    }

    bool trackingOk = tracker->track(frame);

    updateTracks(tracker->getTargets());

    if(trackingOk) {
        trackedFrames++;
        return tracks;
    } else {
        Logger::logDebug("Tracking lost");
        trackedFrames = 0;
        return {};
    }

}

std::vector<Track> DetectAndTrack::updateDetections(std::vector<cv::Rect2d> newDetections) {

    std::vector<Track> newTracks = {};

    for(cv::Rect2d& detection : newDetections){
        std::vector<Track> sortedTracks = tracks;
        std::sort(sortedTracks.begin(),sortedTracks.end(),[&detection](const Track& a, const Track& b){
            return a.getOverlapRatio(detection) > b.getOverlapRatio(detection);
        });

        Track newTrack = Track(trackCount, detection);

        if(!sortedTracks.empty()) {
            Track closestTrack = sortedTracks.front();

            if(closestTrack.getOverlapRatio(detection) > KEEP_TRACK_OVERLAP_THRESHOLD) {
                newTrack.setNumber(closestTrack.getNumber());
                sortedTracks.erase(sortedTracks.begin(), sortedTracks.begin()+1);
            } else
                trackCount++;
        } else
            trackCount++;

        newTracks.push_back(newTrack);

    }

    return newTracks;

}

void DetectAndTrack::updateTracks(std::vector<cv::Rect2d> tracking) {
    for (unsigned long i = 0; i < tracking.size(); ++i) {
        tracks.at(i).setRect(tracking[i]);
    }
}

