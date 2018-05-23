//
// Created by bruno on 01/04/17.
//

#include "memory"
#include "HogDetector.h"
#include "AreaRect.h"

HogDetector::HogDetector() {
    hog.setSVMDetector(cv::HOGDescriptor::getDefaultPeopleDetector());
}

std::vector<cv::Rect2d> HogDetector::getFound() {
    return foundTargets;
}

void HogDetector::detect(std::shared_ptr<cv::Mat> frame) {

    std::vector<cv::Rect> found;

    cv::Mat resizedFrame;

    if(SCALE_FACTOR != 1)
        cv::resize(*frame, resizedFrame, cv::Size(),SCALE_FACTOR, SCALE_FACTOR);
    else
        resizedFrame = *frame;

    hog.detectMultiScale(resizedFrame, found, 1, cv::Size(4, 4), cv::Size(32, 32), 1.05, 1);

    foundTargets.clear();
    for (cv::Rect &t : found)
    {
        foundTargets.push_back(cv::Rect2d((int)(t.x/SCALE_FACTOR), (int)(t.y/SCALE_FACTOR),
                                          (int)(t.width/SCALE_FACTOR), (int)(t.height/SCALE_FACTOR)));
    }

}


