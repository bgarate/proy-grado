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
    paths = config->GetPaths();
    actualPath = myid;
    pathSize = paths[myid].GetPoints().size();

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
    nextPosition.val[0] = 2;
    nextPosition.val[1] = -1;
    nextPosition.val[2] = 0;
    nextRotation = 0;
    previousMarker = -1;
    previousPosition.val[0] = 2;
    previousPosition.val[1] = -1;
    previousPosition.val[2] = 0;
    previousRotation = 0;

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
        //Tengo que volver a mi path?
        if(actualPath != myid && interComm->droneStates[actualPath]->curren_task() != DroneState::CurrentTask::DroneState_CurrentTask_CHARGING){
            actualPath = myid;
            pathSize = paths[myid].GetPoints().size();
        }
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

                    if(interComm->droneStates[myid]->curren_task() == DroneState::CurrentTask::DroneState_CurrentTask_CHARGING){

                        //tiene menos bateria que yo?
                        if(it->second->battery_level() < interComm->droneStates[myid]->battery_level()
                           || (it->second->battery_level() == interComm->droneStates[myid]->battery_level()
                               && it->first < myid)){
                            shouldLeaveCharge = true;
                        }

                    } else {

                        //tengo que cubrir a este drone?
                        if(paths.find(it->first) != paths.end()){
                            actualPath = it->first;
                            pathSize = paths[it->first].GetPoints().size();
                        }
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
                    || interComm->droneStates[myid]->curren_task() == DroneState::CurrentTask::DroneState_CurrentTask_FOLLOWING
                    || interComm->droneStates[myid]->curren_task() == DroneState::CurrentTask::DroneState_CurrentTask_ALERT) {

                interComm->droneStates[myid]->set_curren_task(DroneState::CurrentTask::DroneState_CurrentTask_PATROLING);

            //Si estoy cargando  paso a patrullar
            }if (interComm->droneStates[myid]->curren_task() == DroneState::CurrentTask::DroneState_CurrentTask_CHARGING){

                interComm->droneStates[myid]->set_curren_task(DroneState::CurrentTask::DroneState_CurrentTask_PATROLING);
                startBattryTime = runningTime;

            //Si estoy patrullando o alerta paso a patrullar
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
                previousPosition = nextPosition;
                previousRotation = nextRotation;
                nextMarker = -1;
                nextPosition.val[0] = 2;
                nextPosition.val[1] = -1;
                nextPosition.val[2] = 0;
                nextRotation = 0;


            }else if (interComm->droneStates[myid]->curren_task() == DroneState::CurrentTask::DroneState_CurrentTask_INNACTIVE){

                nextMarker = -1;
                nextPosition.val[0] = 2;
                nextPosition.val[1] = -1;
                nextPosition.val[2] = 0;
                nextRotation = 0;
                previousMarker = -1;
                previousPosition.val[0] = 2;
                previousPosition.val[1] = -1;
                previousPosition.val[2] = 0;
                previousRotation = 0;

            }else if (interComm->droneStates[myid]->curren_task() == DroneState::CurrentTask::DroneState_CurrentTask_ALERT){

                if(nextMarker == 0) {
                    previousMarker = nextMarker;
                    previousPosition = nextPosition;
                    previousRotation = nextRotation;
                }else {
                    nextMarker = 0;
                    nextPosition.val[0] = paths[actualPath].GetPoints().at(0).position.val[0];
                    nextPosition.val[1] = paths[actualPath].GetPoints().at(0).position.val[1];
                    nextPosition.val[2] = paths[actualPath].GetPoints().at(0).position.val[2];
                    nextRotation = paths[actualPath].GetPoints().at(0).rotation;
                }

            }else if (interComm->droneStates[myid]->curren_task() == DroneState::CurrentTask::DroneState_CurrentTask_FOLLOWING){

                previousMarker = nextMarker;
                previousPosition = nextPosition;
                previousRotation = nextRotation;

            } else if (interComm->droneStates[myid]->curren_task() == DroneState::CurrentTask::DroneState_CurrentTask_PATROLING){

                previousMarker = nextMarker;
                previousPosition = nextPosition;
                previousRotation = nextRotation;
                nextMarker = (nextMarker + 1) % pathSize;

                nextPosition.val[0] = paths[actualPath].GetPoints().at(nextMarker).position.val[0];
                nextPosition.val[1] = paths[actualPath].GetPoints().at(nextMarker).position.val[1];
                nextPosition.val[2] = paths[actualPath].GetPoints().at(nextMarker).position.val[2];
                nextRotation = paths[actualPath].GetPoints().at(nextMarker).rotation;
            }else{

                nextMarker = -1;
                nextPosition.val[0] = paths[actualPath].GetPoints().at(0).position.val[0];
                nextPosition.val[1] = paths[actualPath].GetPoints().at(0).position.val[1];
                nextPosition.val[2] = paths[actualPath].GetPoints().at(0).position.val[2];
                nextRotation = paths[actualPath].GetPoints().at(0).rotation;
                previousMarker = -1;
                previousPosition.val[0] = 2;
                previousPosition.val[1] = -1;
                previousPosition.val[2] = 0;
                previousRotation = 0;

            }

            lastChange = runningTime;
        }


        //Actualizar map debugger
        if(runningTime - lastRefreshTime > pirntLapse) {

            double difference = (double)(runningTime - lastChange) / lapseToChange;
            cv::Vec3d *position;
            cv::Vec3d *rotation;
            rotation = new cv::Vec3d(0, 0, 0);

            //Calcular position
            position = new cv::Vec3d((previousPosition.val[0]*(1-difference)
                                      + nextPosition.val[0]*difference)
                    , (previousPosition.val[1]*(1-difference)
                       + nextPosition.val[1]*difference)
                    , 1);
            //Calcular rotation
            float a = previousRotation;
            float b = nextRotation;
            rotation = new cv::Vec3d(0, 0, a + Helpers::angleDifference(b,a)*difference);

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
                mapDebugger->Run(interComm->droneStates,myid,paths[actualPath]);

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