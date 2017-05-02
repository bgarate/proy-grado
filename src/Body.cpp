//
// Created by bruno on 09/04/17.
//

#include <src/proto/message.pb.h>
#include "Body.h"
#include "logging/Logger.h"
#include "Brain.h"
#include "tracking/DetectAndTrack.h"
#include "tracking/MultiTracker.h"
#include "tracking/HogDetector.h"

Body::Body(Hal *hal) {
    this->hal = hal;
    messsageHandler.registerHandler(Message_Type::Message_Type_PING, [this](Message m){this->PingHandler(m);});
    messsageHandler.registerHandler(Message_Type::Message_Type_SHUTDOWN, [this](Message m){this->ShutdownHandler(m);});
}


void Body::setup(std::string brainHost) {
    Logger::getInstance().setSource("BODY");
    communicateWithBrain(brainHost, Brain::BRAIN_SERVE_PORT);
}
void Body::communicateWithBrain(std::string brainHost, unsigned short port) {

    communication.connect(brainHost, port);
    Logger::logInfo("Body has established a connection!");

}

void Body::loop() {
    //test hal init begin
    int aux = 0;
    int auxl=30;
    int auxind = 0;
    int auxarr[2] = {0,180};
    this->hal->takeoff();
    //cv::namedWindow("Front camera", cv::WINDOW_AUTOSIZE);
    cv::Mat frame;
    //test hal init end

    //test track ini begin
    cv::Scalar colors[] = {cv::Scalar(255,0,0), cv::Scalar(0,255,0), cv::Scalar(0,0,255),
                           cv::Scalar(120,120,0), cv::Scalar(0,120,120), cv::Scalar(120,0,120),
                           cv::Scalar(255,0,120), cv::Scalar(0,120,255), cv::Scalar(255,120,0)};
    DetectionAlgorithm* detector = new HogDetector();
    TrackingAlgorithm* tracker = new MultiTracker(MultiTracker::Algorithm::KCF);
    DetectAndTrack detectAndTrack(detector, tracker);
    cv::namedWindow("tracker", cv::WINDOW_AUTOSIZE);
    //test track ini end

    while (true) {
        if(communication.messageAvailable()) {
            Message msg = communication.receive();
            messsageHandler.handle(msg);
        }

        //test hal begin
        aux++;
        if(aux==auxl){
            auxl=60;
            aux=0;
            auxind=(auxind+1) % 2;
        }
        this->hal->hmove(auxarr[auxind],1);
        frame = this->hal->getFrame(Camera::Front);
        //cv::imshow("Front camera", frame);
        //cv::waitKey(1);
        //test hal end

        //test track begin
        //update the tracking result
        std::vector<Track> objects = detectAndTrack.update(frame);

        // draw the tracked object
        for (unsigned i = 0; i < objects.size(); i++)
            rectangle(frame, objects[i].getRect(), colors[objects[i].getNumber() % (sizeof(colors)/sizeof(cv::Scalar))], 2, 1);

        // show image with the tracked object
        imshow("tracker", frame);
        cv::waitKey(1);
        //test track end

        if(should_exit)
            break;

        sleep(0);
    }
}

void Body::PingHandler(Message& msg){

    Ping* ping = msg.mutable_ping();
    if(ping->type() == Ping_PingType_REQUEST) {
        Logger::logInfo("PING REQUEST received");
        ping->set_type(Ping_PingType_ACK);
        communication.send(msg);
        Logger::logInfo("PING ACK sent");
    } else {
        Logger::logInfo("PING ACK received");

    }

}

void Body::ShutdownHandler(Message& msg){

    //DoShutdown* shutdown = msg.mutable_shutdown();
    Logger::logWarning("SHUTDOWN requested");

    should_exit = true;

}


