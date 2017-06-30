#include "BodyTest.h"
#include "src/tracking/DetectAndTrack.h"
#include "src/tracking/MultiTracker.h"
#include "src/tracking/HogDetector.h"
#include "src/VisualDebugger.h"
#include <thread>
#include <unistd.h>

class BodyTestDummy: public BodyTest {

public:

    void InitBodyTest(Hal* hal, Config* config, VisualDebugger* visualDebugger){

    }

    bool BodyTestStep(double deltaTime){

        return true;
    }

    void FinishBodyTest(){

    }

};