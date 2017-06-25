//
// Created by bruno on 01/04/17.
//

#include "DetectAndTrack.h"
#include "../logging/Logger.h"
#include "AreaRect.h"

DetectAndTrack::DetectAndTrack(DetectionAlgorithm *detector, TrackingAlgorithm *tracker) {
    this->detector = detector;
    this->tracker = tracker;
}

bool DetectAndTrack::insideROI(cv::Rect2d r, std::shared_ptr<cv::Mat> frame) {
    cv::Point2d center = cv::Point2d(r.x + r.width / 2, r.y + r.height / 2);
    return (center.x > frame->cols * ROI_MARGIN && center.x < frame->cols * (1 - ROI_MARGIN) &&
       center.y > frame->rows * ROI_MARGIN && center.y < frame->rows* (1 - ROI_MARGIN));
}

std::vector<Track> DetectAndTrack::update(std::shared_ptr<cv::Mat> frame) {

    if(trackedFrames % 50 <= FRAMES_TO_DETECT - 1) {

        detector->detect(frame);

        for(cv::Rect2d found : detector->getFound()) {
            accumulatedFound.push_back(found);
        }

    }

    if(trackedFrames % 50 == FRAMES_TO_DETECT - 1) {

        std::vector<cv::Rect2d> tracksToKeep;

        for(Track& track : tracks){
            cv::Rect2d r = track.getRect();
            if(insideROI(r, frame))
                tracksToKeep.push_back(track.getRect());
        }

        if(!accumulatedFound.empty()) {

            std::vector<cv::Rect2d> detectedAndKept;

            std::copy(tracksToKeep.begin(), tracksToKeep.end(), std::back_inserter(detectedAndKept));
            std::copy(accumulatedFound.begin(), accumulatedFound.end(), std::back_inserter(detectedAndKept));

            std::vector<cv::Rect2d> filteredRects;
            filter_rects(detectedAndKept, tracksToKeep.size(), filteredRects);

            tracker->setTargets(filteredRects, frame);

            int oldTrackingCount = trackCount;
            tracks = updateDetections(filteredRects);

            Logger::logDebug("%u new detections. %u objects kept. %u resulting objects. %u new tracks.") <<
                                                                                   accumulatedFound.size()
                                                                                    << tracksToKeep.size()
                                                                                   << filteredRects.size()
                                                                                   << trackCount - oldTrackingCount;

            accumulatedFound.clear();
        } else {

            if (!tracksToKeep.empty()) {
                Logger::logDebug("No objects detected. Keeping %u from %u tracks in ROI")
                        << tracksToKeep.size() << tracks.size();
            } else {
                if(!tracksToKeep.empty())
                    Logger::logDebug("No objects detected. %u tracks outside ROI") << tracksToKeep.size();
                return {};
            }

            tracker->setTargets(tracksToKeep, frame);
            tracks = updateDetections(tracksToKeep);

        }
    }

    bool trackingOk = tracker->track(frame);

    updateTracks(tracker->getTargets(), frame);

    if(trackingOk) {
        trackedFrames++;
        return tracks;
    } else {
        Logger::logDebug("Tracking lost");
        trackedFrames = 0;
        return {};
    }

}


void DetectAndTrack::filter_rects(std::vector<cv::Rect2d> &candidates, unsigned long tracksKept,std::vector<cv::Rect2d> &objects) {

    // Uses non maxima supression

    std::vector<AreaRect> sortedCandidates;
    for(int i = 0; i < candidates.size();i++)
        sortedCandidates.push_back(AreaRect(candidates[i], i < tracksKept));

    std::sort(sortedCandidates.begin(),sortedCandidates.end(), [](const AreaRect &a,const AreaRect &b) {
        return a.y2 < b.y2;
    });

    std::vector<unsigned int> toRemove;

    while(!sortedCandidates.empty()){

        AreaRect selected = sortedCandidates.back();
        cv::Rect rectToKeep = selected.rect;
        bool isFromKeptTrack = selected.isPreviousTrack;
        toRemove.push_back(sortedCandidates.size()-1);

        unsigned int indexToRemove = 0;

        for(unsigned int i = 0; i < sortedCandidates.size() - 1; i++) {

            AreaRect r = sortedCandidates.at(i);

            double xx1 = std::max(r.x1, selected.x1);
            double yy1 = std::max(r.y1, selected.y1);
            double xx2 = std::min(r.x2, selected.x2);
            double yy2 = std::min(r.y2, selected.y2);

            double width = std::max(0.0, xx2 - xx1 + 1);
            double heigth = std::max(0.0, yy2 - yy1 + 1);

            double overlap = (width*heigth)/selected.area;

            if(overlap > KEEP_TRACK_OVERLAP_THRESHOLD){
                toRemove.push_back(indexToRemove);
                if(isFromKeptTrack != r.isPreviousTrack) {
                    isFromKeptTrack = true;
                    rectToKeep = selected.isPreviousTrack ? rectToKeep : r.rect;
                } else {
                    if(r.rect.width * r.rect.height < selected.rect.height * selected.rect.width)
                        rectToKeep = r.rect;
                }

            } else {
                indexToRemove++;
            }
        }

        objects.push_back(rectToKeep);

        for(unsigned int j : toRemove)
            sortedCandidates.erase(sortedCandidates.begin() + j);

        toRemove.clear();

    }

    for(AreaRect &c : sortedCandidates)
        objects.push_back(c.rect);

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

void DetectAndTrack::updateTracks(std::vector<cv::Rect2d> tracking, std::shared_ptr<cv::Mat> frame) {

    std::vector<unsigned long> tracksToRemove;
    unsigned long indexToRemove = 0;

    for (unsigned long i = 0; i < tracking.size(); ++i) {
        cv::Rect2d t = tracking[i];

        if(t.x + t.width > 0 && t.y + t.height > 0 &&
           t.x < frame->cols && t.y < frame->rows) {
            tracks.at(i).setRect(t);
            indexToRemove++;
        } else {
            tracksToRemove.push_back(indexToRemove);
        }

    }

    if(!tracksToRemove.empty()) {
        Logger::logDebug("%u tracks lost") << tracksToRemove.size();
        for(unsigned long i : tracksToRemove)
            tracks.erase(tracks.begin() + i);

        std::vector<cv::Rect2d> newTargets;
        for(Track t : tracks)
            newTargets.push_back(t.getRect());

        tracker->setTargets(newTargets, frame);

    }
}

