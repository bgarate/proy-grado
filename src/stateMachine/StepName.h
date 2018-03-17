//
// Created by bruno on 04/03/18.
//

#ifndef PROY_GRADO_STEPNAME_H
#define PROY_GRADO_STEPNAME_H

class StepName {
public:
    static std::string TAKING_OFF;
    static std::string LANDING;
    static std::string PATROLLING;
};

std::string StepName::TAKING_OFF = "taking-off";
std::string StepName::LANDING = "landing";
std::string StepName::PATROLLING = "patrolling";

#endif //PROY_GRADO_STEPNAME_H
