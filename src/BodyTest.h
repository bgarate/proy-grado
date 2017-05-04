#ifndef PROY_GRADO_BODYTEST1_H
#define PROY_GRADO_BODYTEST1_H

#include "hal/hal.hpp"

class BodyTest {

public:
    virtual void InitBodyTest(Hal* hal) = 0;

    virtual void BodyTestStep() = 0;

    virtual void FinishBodyTest() = 0;
};


#endif
