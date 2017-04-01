#include "Tracker.h"

#include <opencv2/core/utility.hpp>
#include <opencv2/tracking.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/objdetect/objdetect.hpp>
#include <opencv2/highgui.hpp>
#include <iostream>
#include <cstring>
#include <ctime>


void filter_rects(const std::vector<cv::Rect>& candidates, std::vector<cv::Rect>& objects) {
    size_t i, j;
    for (i = 0; i < candidates.size(); ++i) {
        cv::Rect r = candidates[i];

        for (j = 0; j < candidates.size(); ++j)
            if (j != i && (r & candidates[j]) == r)
                break;

        if (j == candidates.size())
            objects.push_back(r);
    }
}


void find(cv::Mat frame) {

    cv::HOGDescriptor hog;

    std::vector<cv::Rect> found, found_filtered;

    hog.setSVMDetector(cv::HOGDescriptor::getDefaultPeopleDetector());
    hog.detectMultiScale(frame, found, 0, cv::Size(8, 8), cv::Size(32, 32), 1.05, 2);

    filter_rects(found, found_filtered);
}

void track() {

    // set the default tracking algorithm
    // MEDIANFLOW, MIL, BOOSTING, KCF, TLD
    //
    std::string trackingAlg = "MEDIANFLOW";

    // create the tracker
    cv::MultiTracker trackers(trackingAlg);

    // container of the tracked objects
    std::vector<cv::Rect2d> objects;

    cv::Rect2d obj1 = *new cv::Rect2d(100,50,80,80);

    objects.push_back(obj1);

    // set input video
    std::string video = "/home/bruno/proy-grado/video01.mp4";
    cv::VideoCapture cap(video);
    cv::Mat frame;

    // get bounding box
    cap >> frame;

    // initialize the tracker
    trackers.add(frame, objects);

    // do the tracking
    printf("Start the tracking process, press ESC to quit.\n");
    for (;;) {
        // get frame from the video
        cap >> frame;

        // stop the program if no more images
        if (frame.rows == 0 || frame.cols == 0)
            break;

        //update the tracking result
        trackers.update(frame);

        // draw the tracked object
        for (unsigned i = 0; i < trackers.objects.size(); i++)
            rectangle(frame, trackers.objects[i], cv::Scalar(255, 0, 0), 2, 1);

        // show image with the tracked object
        imshow("tracker", frame);

        //quit on ESC button
        if (cv::waitKey(1) == 27)break;
    }
}


void track2() {


    // set input video
    std::string video = "/home/bruno/proy-grado/video01.mp4";
    cv::VideoCapture cap(video);
    cv::Mat frame;

    // get bounding box
    cap >> frame;

    cv::HOGDescriptor hog;

    hog.setSVMDetector(cv::HOGDescriptor::getDefaultPeopleDetector());

    // do the tracking
    printf("Start the tracking process, press ESC to quit.\n");
    for (;;) {
        // get frame from the video
        cap >> frame;

        // stop the program if no more images
        if (frame.rows == 0 || frame.cols == 0)
            break;

        std::vector<cv::Rect> found, found_filtered;

        hog.detectMultiScale(frame, found, 1, cv::Size(8, 8), cv::Size(32, 32), 1.05, 2);

        filter_rects(found, found_filtered);

        // draw the tracked object
        for (unsigned i = 0; i < found_filtered.size(); i++)
            rectangle(frame, found_filtered[i], cv::Scalar(255, 0, 0), 2, 1);

        // show image with the tracked object
        imshow("tracker", frame);

        //quit on ESC button
        if (cv::waitKey(1) == 27)break;
    }
}