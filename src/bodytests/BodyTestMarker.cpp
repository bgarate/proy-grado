#include "BodyTest.h"
#include "../debugging/VisualDebugger.h"
#include "../logging/Logger.h"
#include "../landtracking/MarkerTrack.h"
#include <thread>
#include <unistd.h>
#include "../landtracking/MarkerLand.h"

class BodyTestMarker: public BodyTest {

public:

    Hal* hal;
    VisualDebugger* visualDebugger;
    MarkerTrack* markTrack;
    MarkerLand* markerLand;

    bool waitingTakeOff = false;
    bool tookOff = false;
    bool forward = true;

    float forwardpitch = 0.05;

    void InitBodyTest(Hal* hal, Config* config, VisualDebugger* visualDebugger){

        this->hal = hal;

        this->visualDebugger = visualDebugger;

        this->markTrack = new MarkerTrack(config);

        this->markerLand = new MarkerLand(config);

        Logger::logInfo("Bateria: %u") << hal->bateryLevel();


    }

    bool BodyTestStep(double deltaTime){

        std::shared_ptr<cv::Mat> frame = hal->getFrame(Camera::Front);
        visualDebugger->setSubStatus("");

        if (hal->getState() == State::Landed && !tookOff) {//DESPEGAR

            // Despegar
            visualDebugger->writeConsole("Despegar");
            hal->takeoff();
            tookOff = true;
            waitingTakeOff = true;
            return true;


        } else if (waitingTakeOff &&
                   (hal->getState() == State::Hovering || hal->getState() == State::Flying)) {//DESPEGADO

            visualDebugger->writeConsole("Despegado");
            // Despegado
            waitingTakeOff = false;

            visualDebugger->setSubStatus("Subiendo");

        } else if(!waitingTakeOff) {//ATERRIZAR EN PLATAFORMA
            if (frame != NULL) {

                hal->setCameraTilt(Camera::Bottom);

                std::vector<cv::Point> squarePoints = markTrack->Track(frame);
                visualDebugger->setSquareTracks(squarePoints);

                if(squarePoints.size()>0){
                    cv::Point p = squarePoints[0];
                    int a = 1;
                }


                cv::Point frameSize(frame->size().width,frame->size().height);

                LandMoveCommand command = markerLand->land(squarePoints, frameSize,hal->getAltitude());

                if (command.state == LandingState::Inactive)
                        visualDebugger->setSubStatus("Inactivo");
                else if (command.state == LandingState::Centring)
                        visualDebugger->setSubStatus("Centrando");
                else if (command.state == LandingState::Landing)
                        visualDebugger->setSubStatus("Aterrizando");

                if(std::abs(command.roll+command.pitch+command.yaw)>0.0001){
                    forward=false;
                }

                if(command.land){
                    visualDebugger->setSubStatus("Aterrizando");
                    hal->land();
                    return false;
                }else if(!forward){

                    hal->move((int)(command.roll*100),(int)(command.pitch*100), (int)(-command.yaw * 100),(int)(command.gaz * 100));

                    //debug
                    std::cout << "Yaw: " << command.yaw << "    Pitch: " << command.pitch << "  Roll: " << command.roll << std::endl;
                } else {
                    int gaz = 0;
                    if(hal->getAltitude()<2.5)
                        gaz=5;

                    hal->move(0,(int)(forwardpitch*100), 0,gaz);

                    //debug
                    std::cout << "Forwarding" << std::endl;
                }
            }

        }

        return true;
    }

    void FinishBodyTest(){
        if(hal->getState() != State::Landed)
            hal->land();
    }

};