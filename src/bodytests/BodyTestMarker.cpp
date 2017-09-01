#include "BodyTest.h"
#include "../VisualDebugger.h"
#include "../logging/Logger.h"
#include "src/landtracking/MarkerTrack.h"
#include <thread>
#include <unistd.h>
#include "src/landtracking/MarkerLand.h"

class BodyTestMarker: public BodyTest {

public:

    Hal* hal;
    VisualDebugger* visualDebugger;
    MarkerTrack* markTrack;
    MarkerLand* markerLand;

    bool waitingTakeOff = false;
    bool tookOff = true;//= false;
    bool forward = true;
    bool rmovemode = false;

    float forwardpitch = 0.05;

    void InitBodyTest(Hal* hal, Config* config, VisualDebugger* visualDebugger){

        this->hal = hal;

        this->visualDebugger = visualDebugger;

        this->markTrack = new MarkerTrack();

        this->markerLand = new MarkerLand();

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

            hal->rmove(0, 0, -0.3, 0);
            rmovemode=true;
            visualDebugger->setSubStatus("Subiendo");

        }
        //FIN DE MOVIMIENTO
         else if (rmovemode && !hal->isRmoving()) {

            rmovemode = false;
            //mover adelante hasta encontrar la plataforma
            //hal->move(0,(int)forwardpitch*100, 0,0);
            //hal->rmove(5, 0, 0, 0);
            //visualDebugger->setSubStatus("Adelante");

        }else if(!rmovemode && !waitingTakeOff) {//ATERRIZAR EN PLATAFORMA
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

                    //hal->move((int)(command.roll*100),(int)(command.pitch*100), (int)(-command.yaw * 100),(int)(command.gaz * 100));
                    //std::cout << "Pitch: " << (int)(command.pitch*100) << " Roll: " << (int)(command.roll*100) << std::endl;
                    std::cout << "Yaw: " << command.yaw << "    Pitch: " << command.pitch << "  Roll: " << command.roll << std::endl;
                } else {
                    //hal->move(0,(int)(forwardpitch*100), 0,0);
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