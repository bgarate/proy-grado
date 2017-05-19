#ifndef PROY_GRADO_BODYTEST1_H
#define PROY_GRADO_BODYTEST1_H

#include "src/hal/hal.hpp"

class BodyTest {

public:
    virtual void InitBodyTest(Hal* hal) = 0;

    virtual bool BodyTestStep(double deltaTime) = 0;

    virtual void FinishBodyTest() = 0;
};


#endif
