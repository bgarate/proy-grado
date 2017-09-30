#include "CombC4CascadeDetector.h"

//
// Created by bruno on 01/04/17.
//

#include "memory"
#include "CascadeDetector.h"
#include "AreaRect.h"
#include "C4Detector.h"

CombC4CascadeDetector::CombC4CascadeDetector() {
    c4Detector = new C4Detector();
    cascadeDetector = new CascadeDetector();
}

std::vector<cv::Rect2d> CombC4CascadeDetector::getFound() {
    return foundTargets;
}

//Esta en las dos por overlap
/*void CombC4CascadeDetector::detect(std::shared_ptr<cv::Mat> frame) {

    foundTargets.clear();

    c4Detector->detect(frame);
    std::vector<cv::Rect2d> c4Found = c4Detector->getFound();

    cascadeDetector->detect(frame);
    std::vector<cv::Rect2d> cascadeFound = cascadeDetector->getFound();

    /////////////////Filtrar
    const double KEEP_TRACK_OVERLAP_THRESHOLD = 0.2;

    std::vector<unsigned int> toRemove;

    while(!c4Found.empty() && c4Found.size() < 100){
    //for(unsigned int k = 0; k < c4Found.size(); k++) {

        cv::Rect2d selected = c4Found.back();
        cv::Rect rectToKeep = selected;
        //toRemove.push_back(c4Found.size()-1);
        c4Found.erase(c4Found.end());

        unsigned int overlaped = 0;

        for(unsigned int i = 0; i < cascadeFound.size(); i++) {

            cv::Rect2d r = cascadeFound.at(i);

            double xx1 = std::max(r.x, selected.x);
            double yy1 = std::max(r.y, selected.y);
            double xx2 = std::min(r.x + r.width, selected.x + selected.width);
            double yy2 = std::min(r.y +r.height, selected.y + selected.height);

            double width = std::max(0.0, xx2 - xx1 + 1);
            double heigth = std::max(0.0, yy2 - yy1 + 1);

            double overlap = (width*heigth)/std::min(selected.area(),r.area());

            if(overlap > KEEP_TRACK_OVERLAP_THRESHOLD){

                overlaped++;

            } else {
                //indexToRemove++;
            }
        }

        if(overlaped>0)
            foundTargets.push_back(rectToKeep);

        //for(unsigned int j : toRemove)
        //    c4Found.erase(c4Found.begin() + j);


        toRemove.clear();
    }
}*/

//Esta en las dos por sub detects
void CombC4CascadeDetector::detect(std::shared_ptr<cv::Mat> frame) {

    foundTargets.clear();

    //c4Detector->detect(frame);
   // std::vector<cv::Rect2d> c4Found = c4Detector->getFound();

    cascadeDetector->detect(frame);
    std::vector<cv::Rect2d> cascadeFound = cascadeDetector->getFound();



    for(unsigned int i = 0; i < cascadeFound.size(); i++) {

        cv::Mat* cropped = new cv::Mat(*frame, (cv::Rect)cascadeFound.at(i));

        c4Detector->detect(std::shared_ptr<cv::Mat>(cropped));
        std::vector<cv::Rect2d> c4Found = c4Detector->getFound();
        if(c4Found.size()>0){
            cv::Rect2d r = cascadeFound.at(i);
            foundTargets.push_back(r);
        }
    }

}


