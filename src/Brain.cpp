//
// Created by bruno on 09/04/17.
//

#include <src/proto/dronestate.pb.h>
#include "debugging/MapDebugger.h"
#include "utils/Helpers.h"
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
    interComm->setupInterComm(config, true);

    brainComm = new BrainComm();
    brainComm->setupBrainComm(config);

    this->lowBatteryLevel = config->Get(ConfigKeys::Brain::LowBatteryLevel);
    this->critialBatteryLevel = config->Get(ConfigKeys::Brain::CriticalBatteryLevel);

    Logger::getInstance().setSource("BRAIN");

    //MapDebugger
    mapEnabled = config->Get(ConfigKeys::Debugging::MapDebuggerInBrain);
    if(mapEnabled){
        world = config->GetWorld();
        mapDebugger = new MapDebugger(config, &world);
        mapDebugger->Init();
    }

    //Simulated movement
    path = config->GetPaths();
    size = path[myid].GetPoints().size();
    nextMarker = 0;

};

void Brain::loop() {

    chrono::steady_clock::time_point startTime = chrono::steady_clock::now();
    chrono::steady_clock::time_point lastTime = startTime;
    chrono::steady_clock::time_point newTime = startTime;

    //Comienzo inactivo
    interComm->droneStates[myid]->set_curren_task(DroneState::CurrentTask::DroneState_CurrentTask_PATROLING);
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
    nextMarker = -1;
    previousMarker = -1;

    //Simulación de bateria
    long startBattryTime = std::chrono::duration_cast<std::chrono::microseconds>(chrono::steady_clock::now() - startTime).count();
    double batteryLevel = 100;

    //COMPORAMIENTO SIMULADO VARIABLES
    srand(time(NULL));
    int range = 10 - 3 + 1;
    int taskLapse = (rand() % range + 3) * 1000 * 1000;
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


        ////COMPORTAMIENTO SIMULADO START

        //Actualizar battery level
        if(interComm->droneStates[myid]->curren_task() == DroneState::CurrentTask::DroneState_CurrentTask_CHARGING && nextMarker == previousMarker){

            double diff = (double)(runningTime - taskStartTime) / chargeLapse;

            batteryLevel = diff *100;
            if(interComm->droneStates[myid]->battery_level() < (int)batteryLevel)
                interComm->droneStates[myid]->set_battery_level((int)batteryLevel);

        } else {

            double diff = (double)(runningTime - startBattryTime) / batteryDuration;
            if(diff > 1)
                batteryLevel = 0;
            else
                batteryLevel = 100 - diff *100;
            interComm->droneStates[myid]->set_battery_level((int)batteryLevel);
        }


        //Tengo que cargar?
        bool shouldCharge = (batteryLevel < critialBatteryLevel) || (batteryLevel < lowBatteryLevel && interComm->droneStates[myid]->curren_task() == DroneState::CurrentTask::DroneState_CurrentTask_PATROLING);
        // hay alguien cargando? alguien siguiendo?
        bool someoneFollowing = false;
        bool someoneCharging = false;
        bool shouldLeaveCharge = false;
        for (std::map<int, DroneState*>::iterator it=interComm->droneStates.begin(); it!=interComm->droneStates.end(); ++it) {
            if(it->first != myid){

                //alguien following
                if (it->second->curren_task() == DroneState::CurrentTask::DroneState_CurrentTask_FOLLOWING) { someoneFollowing = true; }
                //alguien cargando
                if (it->second->curren_task() == DroneState::CurrentTask::DroneState_CurrentTask_CHARGING) {
                    someoneCharging = true;
                    //tiene menos bateria que yo?
                    if(it->second->battery_level() < interComm->droneStates[myid]->battery_level()
                            || (it->second->battery_level() == interComm->droneStates[myid]->battery_level()
                                && it->first < myid)){
                        shouldLeaveCharge = true;
                    }
                }
            }
        }

        //Si hay algien sigiendo y yo estoy alerta o patruyando cambio a alerta
        if(someoneFollowing &&
                (interComm->droneStates[myid]->curren_task() == DroneState::CurrentTask::DroneState_CurrentTask_ALERT
                 || interComm->droneStates[myid]->curren_task() == DroneState::CurrentTask::DroneState_CurrentTask_PATROLING)){

            interComm->droneStates[myid]->set_curren_task(DroneState::CurrentTask::DroneState_CurrentTask_ALERT);


        //Si tengo que cargar, no lo estoy haciendo y no hay nadie cargando voy a cargar
        } else if (interComm->droneStates[myid]->curren_task() != DroneState::CurrentTask::DroneState_CurrentTask_CHARGING
                    && !someoneCharging && shouldCharge){

            //Paso a charging
            interComm->droneStates[myid]->set_curren_task(DroneState::CurrentTask::DroneState_CurrentTask_CHARGING);

            //Actualizo lapso y start time
            taskLapse = chargeLapse;
            taskStartTime = runningTime;


        //Si hay alguien cargando con más prioridad y yo tambien estoy cargando dejo de hacerlo
        } else if(shouldLeaveCharge && interComm->droneStates[myid]->curren_task() == DroneState::CurrentTask::DroneState_CurrentTask_CHARGING){

            interComm->droneStates[myid]->set_curren_task(DroneState::CurrentTask::DroneState_CurrentTask_PATROLING);

            taskLapse = (rand() % range + 3) * 1000 * 1000;
            taskStartTime = runningTime;


        //Si no tengo que estar alerta o terminó el lapso de mi tarea cambio de estado
        } else if(interComm->droneStates[myid]->curren_task() == DroneState::CurrentTask::DroneState_CurrentTask_ALERT
                    ||  interComm->droneStates[myid]->curren_task() == DroneState::CurrentTask::DroneState_CurrentTask_INNACTIVE
                    || runningTime - taskStartTime > taskLapse) {

            //Si estoy inactivo o following  paso a patrullar
            if(interComm->droneStates[myid]->curren_task() == DroneState::CurrentTask::DroneState_CurrentTask_INNACTIVE
                    || interComm->droneStates[myid]->curren_task() == DroneState::CurrentTask::DroneState_CurrentTask_FOLLOWING) {

                interComm->droneStates[myid]->set_curren_task(DroneState::CurrentTask::DroneState_CurrentTask_PATROLING);

            //Si estoy cargando  paso a patrullar
            }if (interComm->droneStates[myid]->curren_task() == DroneState::CurrentTask::DroneState_CurrentTask_CHARGING){

                interComm->droneStates[myid]->set_curren_task(DroneState::CurrentTask::DroneState_CurrentTask_PATROLING);
                startBattryTime = runningTime;

            //Si estoy patrullando
            } else {

                //Decido aleatroriamente paso a following o sigo patrullando
                if(rand() % 5 == 1) {
                    //Paso a following
                    interComm->droneStates[myid]->set_curren_task(DroneState::CurrentTask::DroneState_CurrentTask_FOLLOWING);
                }
            }

            //Actualizo lapso y start time
            taskLapse = (rand() % range + 3) * 1000 * 1000;
            taskStartTime = runningTime;
        }


        //Movimiento simulado
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
                    nextMarker = (nextMarker + 1) % size;

            }else{

                    nextMarker = -1;
                    previousMarker = -1;
                    break;
            }

            lastChange = runningTime;
        }


        //Actualizar map debugger
        if(runningTime - lastRefreshTime > pirntLapse) {

            double difference = (double)(runningTime - lastChange) / lapseToChange;
            cv::Vec3d *position;
            cv::Vec3d *rotation;
            rotation = new cv::Vec3d(0, 0, 0);

            if (previousMarker == -1 && nextMarker == -1) {

                position = new cv::Vec3d(2, -1, 0);
                rotation = new cv::Vec3d(0, 0, 0);

            } else if (previousMarker == -1) {

                position = new cv::Vec3d((2 * (1 - difference)
                            + path[myid].GetPoints().at(nextMarker).Postion.val[0] * difference)
                        ,(-1 * (1 - difference)
                            + path[myid].GetPoints().at(nextMarker).Postion.val[1] * difference)
                        , 1);

                cv::Vec3d aux(2, -1, 0);
                aux = path[myid].GetPoints().at(nextMarker).Postion - aux;
                if(aux.val[0] != 0){
                    double edge = atan2 (aux.val[0],aux.val[1]) * 180 / M_PI;
                    rotation = new cv::Vec3d(0, 0, edge);
                }

            } else if(nextMarker == -1){

                position = new cv::Vec3d((path[myid].GetPoints().at(previousMarker).Postion.val[0]*(1-difference)
                            + 2 *difference)
                        , (path[myid].GetPoints().at(previousMarker).Postion.val[1]*(1-difference)
                            + -1 *difference)
                        , 1);

                cv::Vec3d aux(2, -1, 0);
                aux = aux - path[myid].GetPoints().at(previousMarker).Postion;
                if(aux.val[0] != 0){
                    double edge = atan2 (aux.val[0],aux.val[1]) * 180 / M_PI;
                    rotation = new cv::Vec3d(0, 0, edge);
                }

            }else{

                position = new cv::Vec3d((path[myid].GetPoints().at(previousMarker).Postion.val[0]*(1-difference)
                            + path[myid].GetPoints().at(nextMarker).Postion.val[0]*difference)
                        , (path[myid].GetPoints().at(previousMarker).Postion.val[1]*(1-difference)
                            + path[myid].GetPoints().at(nextMarker).Postion.val[1]*difference)
                        , 1);

                float a = path[myid].GetPoints().at(previousMarker).Rotation;
                float b = path[myid].GetPoints().at(nextMarker).Rotation;

                rotation = new cv::Vec3d(0, 0, a + Helpers::angleDifference(b,a)*difference);

            }

            DroneState_Point *p = new DroneState_Point();
            p->set_x(position->val[0]);
            p->set_y(position->val[1]);
            p->set_z(position->val[2]);
            interComm->droneStates[myid]->set_allocated_position(p);

            DroneState_Point *r = new DroneState_Point();
            r->set_x(rotation->val[0]);
            r->set_y(rotation->val[1]);
            r->set_z(rotation->val[2]);
            interComm->droneStates[myid]->set_allocated_rotation(r);

            if(mapEnabled)
                mapDebugger->Run(interComm->droneStates,myid,path[myid]);

            lastRefreshTime = runningTime;
        }


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