//
// Created by bruno on 18/05/17.
//

#include <zconf.h>
#include "../logging/Logger.h"
#include "../tracking/DetectAndTrack.h"
#include "../tracking/HogDetector.h"
#include "../tracking/MultiTracker.h"
#include "../debugging/VisualDebugger.h"
#include "BodyTest.h"
#include "../hal/hal.hpp"
//#include "../../lib/ORB_SLAM2/include/System.h"

class BodyTestRmove2 : public BodyTest {

    typedef struct DirectionTime {
        DirectionTime():x(0),y(0),z(0),h(0){};
        DirectionTime(double x, double y, double z):x(x), y(y),z(0), h(1){}
        DirectionTime(double x, double y, double z, double h):x(x), y(y) ,z(z), h(h){}
        double x;
        double y;
        double z;
        double h;
    } DirectionTime;

    //PATRON DE VIGILANCIA
    /*std::vector<DirectionTime> sequence = {DirectionTime(0,0,-1,0),
                                           DirectionTime(4,0,0,0),
                                           DirectionTime(0,0,0,-1.5708),
                                           DirectionTime(0,0,0,4.71239),
                                           DirectionTime(8,0,0,0),
                                           DirectionTime(0,0,0,-1.5708),
                                           DirectionTime(0,0,0,4.71239),
                                           DirectionTime(4,0,0,0)
    };*/
    std::vector<DirectionTime> sequence = {DirectionTime(0,0,-1,0),
                                           DirectionTime(6,0,0,0),
                                           //DirectionTime(0,0,0,-1.5708),
                                           DirectionTime(0,0,0,(M_PI/2)*3*1.1),
                                           DirectionTime(10,0,0,0),
                                           DirectionTime(0,0,0,(-1.5708)*1.2),
                                           DirectionTime(6,0,0,0),
                                           DirectionTime(0,0,0,(-1.5708)*0.8),
                                           DirectionTime(8,0,0,0),
                                           DirectionTime(0,0,0,-1.5708)
    };


    Hal* hal;

    bool waitingTakeOff = false;
    bool waitingLanding = false;
    bool tookOff = false;
    bool rmovemode = false;

    DetectionAlgorithm* detector;
    TrackingAlgorithm* tracker;
    DetectAndTrack* detectAndTrack;

    bool inmc = false;

    VisualDebugger* visualDebugger;

    //O
    /*ORB_SLAM2::System* SLAM;
    long runningTime = 0;*/

    void InitBodyTest(Hal *hal, Config* config, VisualDebugger* visualDebugger) override {

        this->hal = hal;

        detector = new HogDetector();
        tracker = new MultiTracker(MultiTracker::Algorithm::KCF);
        detectAndTrack =  new DetectAndTrack(detector, tracker);

        this->visualDebugger = visualDebugger;

        Logger::logInfo("Bateria: %u") << hal->bateryLevel();

        //O
        /*const std::string voc = "/home/santy/Escritorio/Proyecto/Git/proy-grado/lib/ORB_SLAM2/Vocabulary/ORBvoc.txt";
        const std::string settings ="/home/santy/Escritorio/Proyecto/Git/proy-grado/lib/ORB_SLAM2/Examples/Monocular/TUM1.yaml";
        SLAM = new ORB_SLAM2::System(voc,settings, ORB_SLAM2::System::MONOCULAR,true);*/
    }

    bool BodyTestStep(double deltaTime) override {

        std::shared_ptr<cv::Mat> frame = hal->getFrame(Camera::Front);

        //O
        /*if(frame != NULL && frame->rows > 0 & frame->cols > 0) {
            cv::Mat grey;
            cv::cvtColor(*frame, grey, CV_BGR2GRAY);
            runningTime += deltaTime;
            SLAM->TrackMonocular(grey, runningTime / 1000.0);
        }*/

        if (frame != NULL) {
            visualDebugger->setFrame(frame);
        }

        //DESPEGAR
        if (hal->getState() == State::Landed && !tookOff) {
            // Despegar
            Logger::logError("Despegar");
            visualDebugger->writeConsole("Despegar");
            hal->takeoff();
            tookOff = true;
            waitingTakeOff = true;
            return true;

        //DESPEGADO
        } else if (waitingTakeOff &&
                   (hal->getState() == State::Hovering || hal->getState() == State::Flying)) {
            Logger::logError("Despegado");
            visualDebugger->writeConsole("Despegado");
            // Despegado
            waitingTakeOff = false;

        //ATERRIZADO
        } else if (waitingLanding && hal->getState() == State::Landed) {
            // Aterrizado
            Logger::logError("Aterrizado");
            visualDebugger->writeConsole("Aterrizado");
            return false;

        //ATERRIZAR
        } else if (sequence.size() == 0 && rmovemode && !hal->isRmoving() && !waitingLanding) {
            // Aterrizar
            Logger::logError("Aterrizar");
            visualDebugger->writeConsole("Aterrizar");
            hal->land();
            waitingLanding = true;
            return true;

        //FIN DE MOVIMIENTO
        } else if (rmovemode && !hal->isRmoving() && !waitingLanding) {
            rmovemode = false;

        //MOVERSE
        } else if (!rmovemode && !waitingLanding && !waitingTakeOff) {

            DirectionTime directionTime = sequence.front();
            sequence.erase(sequence.begin());
            hal->rmove(directionTime.x, directionTime.y, directionTime.z, directionTime.h);
            std::cout << "move to: " << directionTime.x << " " << directionTime.y << " " << directionTime.z << " " << directionTime.h << std::endl;

            rmovemode=true;
            Logger::logError("Do rmove");

            if (frame != NULL) {
                //test track begin
                //update the tracking result
                std::vector<Track> objects = detectAndTrack->update(frame);
                visualDebugger->setTracks(objects);
            }

        } else if (rmovemode){

            if (frame != NULL) {
                //test track begin
                //update the tracking result
                std::vector<Track> objects = detectAndTrack->update(frame);
                visualDebugger->setTracks(objects);
            }

        }

        return true;

    }

    void FinishBodyTest() override {
        if(hal->getState() != State::Landed)
            hal->land();
    }

};