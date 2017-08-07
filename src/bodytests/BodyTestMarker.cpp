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
    bool tookOff = false;

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
        } else {
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


                if(command.land){
                    visualDebugger->setSubStatus("Aterrizando");
                    return false;
                } else /*if (command.roll != 0 || command.pitch != 0 || command.yaw != 0 || command.gaz != 0 )*/ {

                    hal->move((int)(command.roll*100),(int)(command.pitch*100), (int)(-command.yaw * 100),(int)(command.gaz * 100));
                    //std::cout << "Pitch: " << (int)(command.pitch*100) << " Roll: " << (int)(command.roll*100) << std::endl;
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