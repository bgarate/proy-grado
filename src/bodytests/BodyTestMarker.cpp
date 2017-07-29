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
    void InitBodyTest(Hal* hal, Config* config, VisualDebugger* visualDebugger){

        this->hal = hal;

        this->visualDebugger = visualDebugger;

        this->markTrack = new MarkerTrack();

        this->markerLand = new MarkerLand();

        Logger::logInfo("Bateria: %u") << hal->bateryLevel();


    }

    bool BodyTestStep(double deltaTime){

        std::shared_ptr<cv::Mat> frame = hal->getFrame(Camera::Front);

        if (frame != NULL) {

            hal->setCameraTilt(Camera::Bottom);

            std::vector<cv::Point> squarePoints = markTrack->Track(frame);
            visualDebugger->setSquareTracks(squarePoints);

            cv::Point frameSize(frame->size().width,frame->size().height);

            LandMoveCommand command = markerLand->land(squarePoints, frameSize);

            if(command.land){
                hal->land();
            } else {
                hal->move((int)(command.roll*100),(int)(command.pitch*100), (int)(command.yaw * 100),/*gaz*/0);
            }
        }

        return true;
    }

    void FinishBodyTest(){

    }

};