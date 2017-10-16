//
// Created by bruno on 09/04/17.
//

#include <src/proto/message.pb.h>
#include "messages/MessageBuilder.h"
#include "messages/Broadcaster.h"
#include "Brain.h"
#include "logging/Logger.h"
#include "src/config/Config.h"
#include "src/config/ConfigKeys.h"

namespace chrono = std::chrono;

Brain::Brain() {
}

void Brain::setup(Config* config) {

    this->config = config;

    interComm = new InterCommunication();

    interComm->setupInterComm(config);

    Logger::getInstance().setSource("BRAIN");

};

void Brain::loop() {
    chrono::steady_clock::time_point startTime = chrono::steady_clock::now();
    chrono::steady_clock::time_point lastTime = startTime;
    chrono::steady_clock::time_point newTime = startTime;

    Logger::logInfo("Advertisement received");

    while (true) {
        lastTime = newTime;
        newTime = chrono::steady_clock::now();

        deltaTime = std::chrono::duration_cast<std::chrono::microseconds>(newTime - lastTime).count();
        runningTime = std::chrono::duration_cast<std::chrono::microseconds>(newTime - startTime).count();

        interComm->interCommStep(runningTime, deltaTime);

        if(should_exit) {
            break;
        }

        usleep(config->Get(ConfigKeys::Body::SleepDelay));
    }
}

void Brain::shutdown() {
    should_exit = true;
    interComm->shutdownInterComm();
}

void Brain::cleanup() {

}