//
// Created by bruno on 09/04/17.
//

#include "proto/message.pb.h"
#include "communication/BrainComm.h"
#include "Brain.h"
#include "logging/Logger.h"
#include "config/Config.h"
#include "config/ConfigKeys.h"

namespace chrono = std::chrono;

Brain::Brain() {
}

void Brain::setup(Config* config) {

    this->config = config;

    this->myid = config->Get(ConfigKeys::Drone::Id);

    interComm = new InterComm();
    interComm->setupInterComm(config);

    brainComm = new BrainComm();
    brainComm->setupBrainComm(config);

    Logger::getInstance().setSource("BRAIN");

};

void Brain::loop() {
    chrono::steady_clock::time_point startTime = chrono::steady_clock::now();
    chrono::steady_clock::time_point lastTime = startTime;
    chrono::steady_clock::time_point newTime = startTime;

    interComm->droneStates[myid]->set_curren_task(DroneState::CurrentTask::DroneState_CurrentTask_PATROLING);

    int aux = 0;

    while (true) {
        lastTime = newTime;
        newTime = chrono::steady_clock::now();

        deltaTime = std::chrono::duration_cast<std::chrono::microseconds>(newTime - lastTime).count();
        runningTime = std::chrono::duration_cast<std::chrono::microseconds>(newTime - startTime).count();

        interComm->interCommStep(runningTime, deltaTime);
        brainComm->brainCommStep(runningTime, deltaTime);

        //Decidir que hacer
        

        //DEBUG: Imprimir estado de la flotilla
        debugDroneStates(runningTime);

        if(should_exit) {
            break;
        }

        usleep(config->Get(ConfigKeys::Body::SleepDelay));
    }
}

void Brain::shutdown() {
    should_exit = true;
    interComm->shutdownInterComm();
    brainComm->shutdownBrainComm();
}

void Brain::cleanup() {

}

void Brain::debugDroneStates(long runningTime){

    if(runningTime - lastDebug > 3000 * 1000) {
        for (std::map<int, DroneState*>::iterator it=interComm->droneStates.begin(); it!=interComm->droneStates.end(); ++it) {

            std::string state = "";
            if (it->second->curren_task() == 0) { state = "INNACTIVE"; }
            if (it->second->curren_task() == 1) { state = "PATROLING"; }
            if (it->second->curren_task() == 2) { state = "FOLLOWING"; }
            if (it->second->curren_task() == 3) { state = "ALERT"; }
            if (it->second->curren_task() == 4) { state = "CHARGING"; }
            std::cout << "The drone " << it->first << " is " << state << '\n';
        }
    }
    lastDebug = runningTime;

}