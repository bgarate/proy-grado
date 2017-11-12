#include "BodyTest.h"
#include "../tracking/DetectAndTrack.h"
#include "../tracking/MultiTracker.h"
#include "../tracking/HogDetector.h"
#include "../debugging/VisualDebugger.h"
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