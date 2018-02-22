//
// Created by bruno on 09/04/17.
//

#include <src/proto/dronestate.pb.h>
#include <src/navigation/NavigationDebugger.h>
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

    //NAVDEB
    world = config->GetWorld();
    navigationDebugger = new NavigationDebugger(config, &world);
    navigationDebugger->Init();
    follower = new MarkerFollower(config, &world);
    path = config->GetPath();
    follower->setPath(path);
    drone = world.getDrones()[0];

    /*simulatedPath = new int[8];
    simulatedPath[0] = 10;
    simulatedPath[1] = 13;
    simulatedPath[2] = 19;
    simulatedPath[3] = 20;
    simulatedPath[4] = 21;
    simulatedPath[5] = 15;
    simulatedPath[6] = 12;
    simulatedPath[7] = 11;*/
    nextMarker = 0;
    //NAVDEB

};

void Brain::loop() {

    chrono::steady_clock::time_point startTime = chrono::steady_clock::now();
    chrono::steady_clock::time_point lastTime = startTime;
    chrono::steady_clock::time_point newTime = startTime;

    //Comienzo inactivo
    interComm->droneStates[myid]->set_curren_task(DroneState::CurrentTask::DroneState_CurrentTask_INNACTIVE);
    DroneState_Point* r = new DroneState_Point();
    r->set_x(0.0);
    r->set_y(0.0);
    r->set_z(0.0);
    interComm->droneStates[myid]->set_allocated_rotation(r);
    DroneState_Point* p = new DroneState_Point();
    p->set_x(2.0);
    p->set_y(-1.0);
    p->set_z(0.0);
    interComm->droneStates[myid]->set_allocated_position(p);


    //COMPORAMIENTO SIMULADO VARIABLES
    int range = 10 - 3 + 1;
    int taskLapse = rand() % range + 3;
    long taskStartTime = std::chrono::duration_cast<std::chrono::microseconds>(chrono::steady_clock::now() - startTime).count();

    //DEBUG
    lastDebug = 0;

    while (true) {
        lastTime = newTime;
        newTime = chrono::steady_clock::now();

        deltaTime = std::chrono::duration_cast<std::chrono::microseconds>(newTime - lastTime).count();
        runningTime = std::chrono::duration_cast<std::chrono::microseconds>(newTime - startTime).count();

        interComm->interCommStep(runningTime, deltaTime);
        //brainComm->brainCommStep(runningTime, deltaTime);

        //DEBUG: Imprimir estado de la flotilla
        //debugDroneStates(runningTime);

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
                    ||  interComm->droneStates[myid]->curren_task() == DroneState::CurrentTask::DroneState_CurrentTask_INNACTIVE
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
                if(rand() % 3 == 0 && !someoneCharging){
                    //Paso a charging
                    interComm->droneStates[myid]->set_curren_task(DroneState::CurrentTask::DroneState_CurrentTask_CHARGING);
                } else if(rand() % 3 == 1) {
                    //Paso a following
                    interComm->droneStates[myid]->set_curren_task(DroneState::CurrentTask::DroneState_CurrentTask_FOLLOWING);
                }//else sigo patrullando
            }

            //Actualizo lapso y start time
            taskLapse = rand() % range + 3;
            taskStartTime = runningTime;
        }

        //NAVDEB
        if(runningTime - lastChange > lapseToChange) {


            if (interComm->droneStates[myid]->curren_task() == DroneState::CurrentTask::DroneState_CurrentTask_CHARGING ) {

                previousMarker = nextMarker;
                nextMarker = -1;

            }else if (interComm->droneStates[myid]->curren_task() == DroneState::CurrentTask::DroneState_CurrentTask_INNACTIVE){

                    nextMarker = -1;
                    previousMarker = -1;

            }else if (interComm->droneStates[myid]->curren_task() == DroneState::CurrentTask::DroneState_CurrentTask_ALERT){

                if(nextMarker == 0)
                    previousMarker = 0;
                else
                    nextMarker = 0;


            }else if (interComm->droneStates[myid]->curren_task() == DroneState::CurrentTask::DroneState_CurrentTask_FOLLOWING){

                previousMarker = nextMarker;

            } else if (interComm->droneStates[myid]->curren_task() == DroneState::CurrentTask::DroneState_CurrentTask_PATROLING){

                    previousMarker = nextMarker;
                    nextMarker = (nextMarker + 1) % pathSize;

            }else{

                          nextMarker = -1;
                    previousMarker = -1;
                    break;

            }

            lastChange = runningTime;
        }


        if(runningTime - lastRefreshTime > pirntLapse) {

            double difference = (runningTime - lastChange) / (1000.0 * 1000.0);

            cv::Vec3d *position;
            if (previousMarker == -1 && nextMarker == -1) {

                position = new cv::Vec3d(2, -1, 0);

            } else if (previousMarker == -1) {

                position = new cv::Vec3d((2 * (1 - difference) +
                                          world.getMarker(simulatedPath[nextMarker])->getPosition().val[0] *
                                          difference), (-1 * (1 - difference) + world.getMarker(
                        simulatedPath[nextMarker])->getPosition().val[1] * difference), 1);

            } else if(nextMarker == -1){

                position = new cv::Vec3d((world.getMarker(simulatedPath[previousMarker])->getPosition().val[0]*(1-difference) + 2 *difference)
                        , (world.getMarker(simulatedPath[previousMarker])->getPosition().val[1]*(1-difference) + -1 *difference)
                        , 1);

            }else{

                position = new cv::Vec3d((world.getMarker(simulatedPath[previousMarker])->getPosition().val[0]*(1-difference) + world.getMarker(simulatedPath[nextMarker])->getPosition().val[0]*difference)
                        , (world.getMarker(simulatedPath[previousMarker])->getPosition().val[1]*(1-difference) + world.getMarker(simulatedPath[nextMarker])->getPosition().val[1]*difference)
                        , 1);

            }
            drone->setPosition(*position);
            std::string state;
            if (interComm->droneStates[myid]->curren_task() == DroneState::CurrentTask::DroneState_CurrentTask_INNACTIVE) { state = "INNACTIVE"; }
            if (interComm->droneStates[myid]->curren_task() == DroneState::CurrentTask::DroneState_CurrentTask_PATROLING) { state = "PATROLING"; }
            if (interComm->droneStates[myid]->curren_task() == DroneState::CurrentTask::DroneState_CurrentTask_FOLLOWING) { state = "FOLLOWING"; }
            if (interComm->droneStates[myid]->curren_task() == DroneState::CurrentTask::DroneState_CurrentTask_ALERT) { state = "ALERT"; }
            if (interComm->droneStates[myid]->curren_task() == DroneState::CurrentTask::DroneState_CurrentTask_CHARGING) { state = "CHARGING"; }
            drone->setState(state);
            navigationDebugger->Run(command, follower->getTargetId(), follower->EstimatedPositions,
                                    follower->EstimatedPoses, path,
                                    follower->PositionsHistory, follower->PredictedPosition,
                                    follower->ProjectedPredictedPosition, follower->FollowTarget);


            lastRefreshTime = runningTime;
        }
        //NAVDEB


        ////COMPORTAMIENTO SIMULADO END


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

    if(runningTime - lastDebug > 1 * 1000  * 1000) {

        std::cout << '\n';
        for (std::map<int, DroneState*>::iterator it=interComm->droneStates.begin(); it!=interComm->droneStates.end(); ++it) {

            std::string state = "";
            if (it->second->curren_task() == DroneState::CurrentTask::DroneState_CurrentTask_INNACTIVE) { state = "INNACTIVE"; }
            if (it->second->curren_task() == DroneState::CurrentTask::DroneState_CurrentTask_PATROLING) { state = "PATROLING"; }
            if (it->second->curren_task() == DroneState::CurrentTask::DroneState_CurrentTask_FOLLOWING) { state = "FOLLOWING"; }
            if (it->second->curren_task() == DroneState::CurrentTask::DroneState_CurrentTask_ALERT) { state = "ALERT"; }
            if (it->second->curren_task() == DroneState::CurrentTask::DroneState_CurrentTask_CHARGING) { state = "CHARGING"; }
            std::cout << "The drone " << it->first << " is " << state
                      << " with position {" << it->second->position().x() << ", " << it->second->position().y () << ", " << it->second->position().z()
                      << "} and rotation {" << it->second->rotation().x() << ", " << it->second->rotation().y () << ", " << it->second->rotation().z() << "}"
                      << "\n";
        }
        std::cout << '\n';

        lastDebug = runningTime;
    }


}