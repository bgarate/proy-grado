//
// Created by bruno on 01/04/17.
//

#include "memory"
#include "CascadeDetector.h"
#include "AreaRect.h"

CascadeDetector::CascadeDetector() {
    if(!cascade.load("pedestrian_cascade_web_LBP.xml"))
        throw new std::runtime_error("Could not load detector");
}

std::vector<cv::Rect2d> CascadeDetector::getFound() {
    return foundTargets;
}

void CascadeDetector::detect(std::shared_ptr<cv::Mat> frame) {

    std::vector<cv::Rect> found;

    if(frame->cols * frame->rows == 0)
        return;

    cv::Mat resizedFrame;

    (*frame).copyTo(resizedFrame);

    if(SCALE_FACTOR != 1)
        cv::resize(resizedFrame, resizedFrame, cv::Size(),SCALE_FACTOR, SCALE_FACTOR);

    //cascade.detectMultiScale(resizedFrame, found, 1.1, 4, 0, cv::Size(40,120),cv::Size(400,1200));
    //cascade.detectMultiScale(resizedFrame, found, 1.1, 8, 0, cv::Size(40,120),cv::Size(400,1200));
    //cascade.detectMultiScale(resizedFrame, found, 2, 14, 0, cv::Size(40,120),cv::Size(400,1200));
    cascade.detectMultiScale(resizedFrame, found, 1.1, 2, 0, cv::Size(40,120),cv::Size(400,1200));

    foundTargets.clear();

    for (cv::Rect &t : found)
    {
        int x = std::max(0,(int)(t.x/SCALE_FACTOR));
        int y = std::max(0,(int)(t.y/SCALE_FACTOR));
        int width = std::min((int)(t.width/SCALE_FACTOR),frame->cols - x);
        int height  = std::min((int)(t.height/SCALE_FACTOR),frame->rows - y);
        cv::Rect2d r = cv::Rect2d(x, y, width,height);
        foundTargets.push_back(r);
    }

}


