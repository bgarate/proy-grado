//
// Created by bruno on 09/04/17.
//

#include <src/proto/dronestate.pb.h>
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

    interComm->droneStates[myid]->set_curren_task(DroneState::CurrentTask::DroneState_CurrentTask_INNACTIVE);

    //COMPORAMIENTO SIMULADO VARIABLES
    int range = 10 - 3 + 1;
    int taskLapse = rand() % range + 3;
    long taskStartTime = std::chrono::duration_cast<std::chrono::microseconds>(chrono::steady_clock::now() - startTime).count();

    while (true) {
        lastTime = newTime;
        newTime = chrono::steady_clock::now();

        deltaTime = std::chrono::duration_cast<std::chrono::microseconds>(newTime - lastTime).count();
        runningTime = std::chrono::duration_cast<std::chrono::microseconds>(newTime - startTime).count();

        interComm->interCommStep(runningTime, deltaTime);
        brainComm->brainCommStep(runningTime, deltaTime);

        ////COMPORTAMIENTO SIMULADO START

        //Tengo que estar alerta? alguien cargando?
        bool someoneFollowing = false;
        bool someoneCharging = false;
        for (std::map<int, DroneState*>::iterator it=interComm->droneStates.begin(); it!=interComm->droneStates.end(); ++it) {
            if(it->first != myid){
                if (it->second->curren_task() == DroneState::CurrentTask::DroneState_CurrentTask_FOLLOWING) { someoneFollowing = true; }
                if (it->second->curren_task() == DroneState::CurrentTask::DroneState_CurrentTask_CHARGING) { someoneCharging = true; }
            }
        }

        //Si hay algien sigiendo y yo estoy alerta o patruyando cambio a alerta
        if(someoneFollowing &&
                (interComm->droneStates[myid]->curren_task() == DroneState::CurrentTask::DroneState_CurrentTask_ALERT
                 || interComm->droneStates[myid]->curren_task() == DroneState::CurrentTask::DroneState_CurrentTask_PATROLING)){

            interComm->droneStates[myid]->set_curren_task(DroneState::CurrentTask::DroneState_CurrentTask_ALERT);

        //Si hay alguien cargando y yo tambien estoy cargando dejo de hacerlo
        } else if(someoneCharging && interComm->droneStates[myid]->curren_task() == DroneState::CurrentTask::DroneState_CurrentTask_CHARGING){

            interComm->droneStates[myid]->set_curren_task(DroneState::CurrentTask::DroneState_CurrentTask_PATROLING);

            taskLapse = rand() % range + 3;
            taskStartTime = runningTime;

        //Si no tengo que estar alerta o terminó el lapso de mi tarea cambio de estado
        } else if(interComm->droneStates[myid]->curren_task() == DroneState::CurrentTask::DroneState_CurrentTask_ALERT
                  || runningTime - taskStartTime > taskLapse * 1000 * 1000) {

            //Si estoy inactivo, following o cargando  paso a patrullar
            if(interComm->droneStates[myid]->curren_task() == DroneState::CurrentTask::DroneState_CurrentTask_INNACTIVE
                    || interComm->droneStates[myid]->curren_task() == DroneState::CurrentTask::DroneState_CurrentTask_CHARGING
                    || interComm->droneStates[myid]->curren_task() == DroneState::CurrentTask::DroneState_CurrentTask_FOLLOWING){

                interComm->droneStates[myid]->set_curren_task(DroneState::CurrentTask::DroneState_CurrentTask_PATROLING);

            //Si estoy patrullando
            } else {

                //Decido aleatroriamente paso a Charging o following
                //Paso a cargar si nadio ya lo está haciendo
                if(rand() % 2 == 0 && !someoneCharging){
                    //Paso a charging
                    interComm->droneStates[myid]->set_curren_task(DroneState::CurrentTask::DroneState_CurrentTask_CHARGING);
                } else {
                    //Paso a following
                    interComm->droneStates[myid]->set_curren_task(DroneState::CurrentTask::DroneState_CurrentTask_FOLLOWING);
                }
            }

            //Actualizo lapso y start time
            taskLapse = rand() % range + 3;
            taskStartTime = runningTime;
        }

        ////COMPORTAMIENTO SIMULADO END

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
            if (it->second->curren_task() == DroneState::CurrentTask::DroneState_CurrentTask_INNACTIVE) { state = "INNACTIVE"; }
            if (it->second->curren_task() == DroneState::CurrentTask::DroneState_CurrentTask_PATROLING) { state = "PATROLING"; }
            if (it->second->curren_task() == DroneState::CurrentTask::DroneState_CurrentTask_FOLLOWING) { state = "FOLLOWING"; }
            if (it->second->curren_task() == DroneState::CurrentTask::DroneState_CurrentTask_ALERT) { state = "ALERT"; }
            if (it->second->curren_task() == DroneState::CurrentTask::DroneState_CurrentTask_CHARGING) { state = "CHARGING"; }
            std::cout << "The drone " << it->first << " is " << state << '\n';
        }
    }
    lastDebug = runningTime;

}