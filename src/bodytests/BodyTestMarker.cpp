#include "BodyTest.h"
#include "../VisualDebugger.h"
#include "../logging/Logger.h"
#include "src/landtracking/MarkerTrack.h"
#include <thread>
#include <unistd.h>

class BodyTestMarker: public BodyTest {

public:

    Hal* hal;
    VisualDebugger* visualDebugger;
    MarkerTrack* markTrack;

    void InitBodyTest(Hal* hal, Config* config, VisualDebugger* visualDebugger){

        this->hal = hal;

        this->visualDebugger = visualDebugger;

        this->markTrack = new MarkerTrack();

        Logger::logInfo("Bateria: %u") << hal->bateryLevel();

    }

    bool BodyTestStep(double deltaTime){

        std::shared_ptr<cv::Mat> frame = hal->getFrame(Camera::Front);

        if (frame != NULL) {

            std::vector<cv::Point> squarePoints = markTrack->Track(frame);
            visualDebugger->setSquareTracks(squarePoints);

        }

        return true;
    }

    void FinishBodyTest(){

    }

};