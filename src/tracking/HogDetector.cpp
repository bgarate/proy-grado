//
// Created by bruno on 01/04/17.
//

#include "HogDetector.h"

HogDetector::HogDetector() {
    hog.setSVMDetector(cv::HOGDescriptor::getDefaultPeopleDetector());
}

std::vector<cv::Rect2d> HogDetector::getFound() {
    return foundTargets;
}

void HogDetector::detect(cv::Mat frame) {

    std::vector<cv::Rect> found;
    std::vector<cv::Rect2d> found_filtered;

    cv::Mat resizedFrame;

    if(SCALE_FACTOR != 1)
        cv::resize(frame, resizedFrame, cv::Size(),SCALE_FACTOR, SCALE_FACTOR);
    else
        resizedFrame = frame;

    hog.detectMultiScale(resizedFrame, found, 1, cv::Size(8, 8), cv::Size(32, 32), 1.05, 2);

    filter_rects(found, found_filtered);

    foundTargets.clear();
    for (cv::Rect2d &t : found_filtered)
    {
        foundTargets.push_back(cv::Rect2d((int)(t.x/SCALE_FACTOR), (int)(t.y/SCALE_FACTOR),
                                          (int)(t.width/SCALE_FACTOR), (int)(t.height/SCALE_FACTOR)));
    }

}
struct AreaRect {

    AreaRect(){}
    AreaRect(cv::Rect r)
    {
        x1 = r.x;
        y1 = r.y;
        x2 = r.x + r.width;
        y2 = r.y + r.height;
        area = r.width * r.height;
        rect = r;
    }
    double x1, x2, y1, y2, area;

    cv::Rect rect;

};



void HogDetector::filter_rects(std::vector<cv::Rect> &candidates, std::vector<cv::Rect2d> &objects) {

    // Uses non maxima supression

    std::vector<AreaRect> sortedCandidates;
    for(cv::Rect r : candidates)
        sortedCandidates.push_back(AreaRect(r));

    std::sort(sortedCandidates.begin(),sortedCandidates.end(), [](const AreaRect &a,const AreaRect &b) {
        return a.y2 < b.y2;
    });

    std::vector<unsigned int> toRemove;

    while(!sortedCandidates.empty()){

        AreaRect selected = sortedCandidates.back().rect;
        objects.push_back(selected.rect);
        toRemove.push_back(sortedCandidates.size()-1);

        for(unsigned int i = 0; i < sortedCandidates.size() - 1; i++) {

            AreaRect r = sortedCandidates.at(i);

            double xx1 = std::max(r.x1, selected.x1);
            double yy1 = std::max(r.y1, selected.y1);
            double xx2 = std::min(r.x2, selected.x2);
            double yy2 = std::min(r.y2, selected.y2);

            double width = std::max(0.0, xx2 - xx1 + 1);
            double heigth = std::max(0.0, yy2 - yy1 + 1);

            double overlap = (width*heigth)/selected.area;

            if(overlap > OVERLAP_THRESHOLD){
                toRemove.push_back(i);
            }
        }

        for(unsigned int j : toRemove)
            sortedCandidates.erase(sortedCandidates.begin() + j);

        toRemove.clear();

    }

    for(AreaRect &c : sortedCandidates)
        objects.push_back(c.rect);

}


