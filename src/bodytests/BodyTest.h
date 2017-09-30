#ifndef PROY_GRADO_BODYTEST1_H
#define PROY_GRADO_BODYTEST1_H

#include "../hal/hal.hpp"
#include "../VisualDebugger.h"

#define BODY_TEST_CLASS(NAME) \
    class NAME

class BodyTest {

public:
    virtual void InitBodyTest(Hal* hal, Config* config, VisualDebugger* visualDebugger) = 0;

    virtual bool BodyTestStep(double deltaTime) = 0;

    virtual void FinishBodyTest() = 0;

};


#endif
