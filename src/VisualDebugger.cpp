//
// Created by bruno on 27/05/17.
//


#include "VisualDebugger.h"

void VisualDebugger::setup(Config *config) {
    this->config = config;
    windowName = config->getName() + " - VisualDebugger";

    if(config->isVisualDebugEnabled())
        cv::namedWindow(windowName, cv::WINDOW_AUTOSIZE);
}

void VisualDebugger::setFrame(std::shared_ptr<cv::Mat> frame) {
    if(frame != NULL)
        this->frame = *frame;
}

void VisualDebugger::setTracks(std::vector<Track> tracks) {

    if(!config->isVisualDebugEnabled())
        return;

    // draw the tracked object
    for (unsigned i = 0; i < tracks.size(); i++)
        rectangle(frame, tracks[i].getRect(),
                  colors[tracks[i].getNumber() % (sizeof(colors)/sizeof(cv::Scalar))], 2, 1);

}

int VisualDebugger::show(){


    if(!config->isVisualDebugEnabled())
        return 0;

    cv::imshow(windowName, frame);
    return cv::waitKey(1);
}
