#include "BodyTest.h"
#include "../tracking/DetectAndTrack.h"
#include "../tracking/MultiTracker.h"
#include "../tracking/HogDetector.h"
#include "../debugging/VisualDebugger.h"
#include <thread>
#include <unistd.h>

class BodyTest1: public BodyTest {
private:
    Hal* hal;
    int aux = 0;
    int auxl=30;
    int auxind = 0;
    int auxarr[2] = {0,180};

    std::thread* t;

    std::shared_ptr<cv::Mat> frame;

    cv::Scalar colors[9];
    DetectionAlgorithm* detector;
    TrackingAlgorithm* tracker;
    DetectAndTrack* detectAndTrack;


public:

    void InitBodyTest(Hal* hal, Config* config, VisualDebugger* visualDebugger){
        this->hal = hal;
        this->hal->takeoff();

        colors[0] = cv::Scalar(255,0,0);
        colors[1] = cv::Scalar(0,255,0);
        colors[2] = cv::Scalar(0,0,255);
        colors[3] = cv::Scalar(120,120,0);
        colors[4] = cv::Scalar(0,120,120);
        colors[5] = cv::Scalar(120,0,120);
        colors[6] = cv::Scalar(255,0,120);
        colors[7] = cv::Scalar(0,120,255);
        colors[8] = cv::Scalar(255,120,0);
        detector = new HogDetector();
        tracker = new MultiTracker(MultiTracker::Algorithm::KCF);
        detectAndTrack =  new DetectAndTrack(detector, tracker);

        cv::namedWindow("tracker", cv::WINDOW_AUTOSIZE);

        this->t = new std::thread(&BodyTest1::deamon, this);
    }

    void deamon(){

        this->hal->move(0,50,0,0);
        sleep(6);
        this->hal->move(0,0,0,0);
        sleep(2);

        while(1){
            this->hal->move(0,-50,0,0);
            sleep(20);
            this->hal->move(0,0,0,0);
            sleep(2);

            this->hal->move(0,50,0,0);
            sleep(20);
            this->hal->move(0,0,0,0);
            sleep(2);
        }
    }

    bool BodyTestStep(double deltaTime){

        //test hal begin
        aux++;
        if(aux==auxl){
            auxl=60;
            aux=0;
            auxind=(auxind+1) % 2;
        }
        //this->hal->hmove(auxarr[auxind],1);

        frame = this->hal->getFrame(Camera::Front);
        //test hal end

        //test track begin
        //update the tracking result
        std::vector<Track> objects = detectAndTrack->update(frame);

        // draw the tracked object
        for (unsigned i = 0; i < objects.size(); i++)
            rectangle(*frame, objects[i].getRect(), colors[objects[i].getNumber() % (sizeof(colors)/sizeof(cv::Scalar))], 2, 1);

        // show image with the tracked object
        imshow("tracker", *frame);
        cv::waitKey(1);
        //test track end

        //release
        //delete frame->data;
        //frame->release();

        return true;
    }

    void FinishBodyTest(){

    }

};