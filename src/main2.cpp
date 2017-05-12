#include <iostream>
#include <opencv2/videoio.hpp>
#include "tracking/DetectAndTrack.h"
#include "tracking/MultiTracker.h"
#include "tracking/HogDetector.h"

cv::Scalar colors[] = {cv::Scalar(255,0,0), cv::Scalar(0,255,0), cv::Scalar(0,0,255),
                       cv::Scalar(120,120,0), cv::Scalar(0,120,120), cv::Scalar(120,0,120),
                       cv::Scalar(255,0,120), cv::Scalar(0,120,255), cv::Scalar(255,120,0)};

void trackFromVideo(){

    // set input video
    std::string video = "/home/santy/Escritorio/Proyecto/Git/proy-grado/out.ogv";
    cv::VideoCapture cap(video);
    cv::Mat frame;

    // get bounding box
    /*for(int i = 0; i < 200; i++)
      cap.grab();*/

    DetectionAlgorithm* detector = new HogDetector();
    TrackingAlgorithm* tracker = new MultiTracker(MultiTracker::Algorithm::KCF);
    DetectAndTrack detectAndTrack(detector, tracker);

    // do the tracking
    printf("Start the tracking process, press ESC to quit.\n");
    for (;;) {
        // get frame from the video
        cap >> frame;

        // stop the program if no more images
        if (frame.rows == 0 || frame.cols == 0)
            break;

        //update the tracking result
        std::vector<Track> objects = detectAndTrack.update(frame);

        // draw the tracked object
        for (unsigned i = 0; i < objects.size(); i++)
            rectangle(frame, objects[i].getRect(), colors[objects[i].getNumber() % (sizeof(colors)/sizeof(cv::Scalar))], 2, 1);

        // show image with the tracked object
        imshow("tracker", frame);

        //quit on ESC button
        if (cv::waitKey(1) == 27)break;
    }
}

void welcome_message() {
    std::cout << "PROYECTO DE GRADO - 2017" << std::endl;
    std::cout << "Inteligencia computacional para la planificación de vuelo de vehículos aéreos no tripulados" << std::endl;
    std::cout << "Santiago Díaz, Bruno Garate, Sergio Nesmachnow, Santiago Iturriaga" << std::endl;
}

int main() {
    welcome_message();
    trackFromVideo();
    return 0;
}