//
// Created by bruno on 30/09/17.
//

#include "BodyTestRegistry.h"
#include "BodyTest1.cpp"
#include "BodyTest2.cpp"
#include "BodyTestDummy.cpp"
#include "BodyTestMarker.cpp"
#include "BodyTestRmove.cpp"
#include "BodyTestRmove2.cpp"
#include "FlightManeuver.cpp"
#include "Follow.cpp"
#include "OpticalFlowObstacleAvoidance.cpp"
#include "PatrolAndFollow.cpp"
#include "TrackMarkers.cpp"

std::map<std::string, BodyTestRegistry::Handler> BodyTestRegistry::bodyTests;
bool BodyTestRegistry::initialized = false;

BodyTest* BodyTestRegistry::Get(std::string name) {

    if(!initialized)
        Initialize();

    return bodyTests.at(name)();
}

void BodyTestRegistry::Register(std::string name, Handler instanceCreator) {
    bodyTests[name] = instanceCreator;
}

std::vector<std::string> BodyTestRegistry::GetAvailableTests() {

    if(!initialized)
        Initialize();

    std::vector<std::string> keys;
    std::transform (bodyTests.begin(), bodyTests.end(), std::back_inserter(keys),
                    [](std::pair<const std::string, Handler>& pair) -> std::string{
                        return std::string(pair.first);
    });
    return keys;
}

void BodyTestRegistry::Initialize() {

    initialized = true;

    REGISTER_BODY_TEST(BodyTest1);
    REGISTER_BODY_TEST(BodyTest2);
    REGISTER_BODY_TEST(BodyTestDummy);
    REGISTER_BODY_TEST(BodyTestMarker);
    REGISTER_BODY_TEST(BodyTestRmove);
    REGISTER_BODY_TEST(BodyTestRmove2);
    REGISTER_BODY_TEST(FlightManeuver);
    REGISTER_BODY_TEST(Follow);
    REGISTER_BODY_TEST(OpticalFlowObstacleAvoidance);
    REGISTER_BODY_TEST(PatrolAndFollow);
    REGISTER_BODY_TEST(TrackMarkers);
}
