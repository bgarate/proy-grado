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

    pads = world.getPads();

};

void Brain::loop() {

    chrono::steady_clock::time_point startTime = chrono::steady_clock::now();
    chrono::steady_clock::time_point lastTime = startTime;
    chrono::steady_clock::time_point newTime = startTime;

    //Comienzo inactivo
    interComm->droneStates[myid]->set_curren_task(DroneState::CurrentTask::DroneState_CurrentTask_INNACTIVE);
    interComm->droneStates[myid]->set_covered_drone_id(0);
    WorldObject* d = world.getDrone(myid);
    DroneState_Point* r = new DroneState_Point();
    r->set_x(d->getRotation().val[0]);
    r->set_y(d->getRotation().val[1]);
    r->set_z(d->getRotation().val[2]);
    interComm->droneStates[myid]->set_allocated_rotation(r);
    DroneState_Point* p = new DroneState_Point();
    p->set_x(d->getPosition().val[0]);
    p->set_y(d->getPosition().val[1]);
    p->set_z(d->getPosition().val[2]);
    interComm->droneStates[myid]->set_allocated_position(p);
    nextMarker = -1;
    nextPosition.val[0] = interComm->droneStates[myid]->position().x();
    nextPosition.val[1] = interComm->droneStates[myid]->position().y();
    nextPosition.val[2] = interComm->droneStates[myid]->position().z();
    nextRotation = interComm->droneStates[myid]->rotation().y();
    previousMarker = -1;
    previousPosition.val[0] = nextPosition.val[0];
    previousPosition.val[1] = nextPosition.val[1];
    previousPosition.val[2] = nextPosition.val[2];
    previousRotation = nextRotation;

    //Simulación de bateria
    long startBattryTime = std::chrono::duration_cast<std::chrono::microseconds>(chrono::steady_clock::now() - startTime).count();
    double batteryLevel = 100;

    //COMPORAMIENTO SIMULADO VARIABLES
    srand(time(NULL));
    int range = 10 - 3 + 1;
    int taskLapse = (rand() % range + 3) * 1000 * 1000;
    long taskStartTime = std::chrono::duration_cast<std::chrono::microseconds>(chrono::steady_clock::now() - startTime).count();

    while (true) {
        lastTime = newTime;
        newTime = chrono::steady_clock::now();

        deltaTime = std::chrono::duration_cast<std::chrono::microseconds>(newTime - lastTime).count();
        runningTime = std::chrono::duration_cast<std::chrono::microseconds>(newTime - startTime).count();

        interComm->interCommStep(runningTime, deltaTime);
        //brainComm->brainCommStep(runningTime, deltaTime);


        ////COMPORTAMIENTO SIMULADO START

        //Actualizar battery level
        if(nextMarker == previousMarker && (interComm->droneStates[myid]->curren_task() == DroneState::CurrentTask::DroneState_CurrentTask_CHARGING
                                            || interComm->droneStates[myid]->curren_task() == DroneState::CurrentTask::DroneState_CurrentTask_CHARGED)){

            double diff = (double)(runningTime - taskStartTime) / chargeLapse;

            if(diff > 1)
                batteryLevel = 100;
            else
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

        // hay alguien cargando? alguien siguiendo?
        //Tengo que cargar?
        bool shouldCharge = (batteryLevel < critialBatteryLevel) || (batteryLevel < lowBatteryLevel && interComm->droneStates[myid]->curren_task() == DroneState::CurrentTask::DroneState_CurrentTask_PATROLING);
        bool someoneFollowing = false;
        int chargingCount = 0;
        bool shouldLeaveCharge = 0;
        bool someoneCovering = false;
        for (std::map<int, DroneState*>::iterator it=interComm->droneStates.begin(); it!=interComm->droneStates.end(); ++it) {
            if(it->first != myid){

                //alguien following
                if (it->second->curren_task() == DroneState::CurrentTask::DroneState_CurrentTask_FOLLOWING) {
                    someoneFollowing = true;
                    alertid = it->first;
                }
                //alguien me esta cubriendo
                if (it->second->covered_drone_id() == myid) {
                    someoneCovering = true;
                }

                //alguien cargando
                if (it->second->curren_task() == DroneState::CurrentTask::DroneState_CurrentTask_CHARGING
                        || it->second->curren_task() == DroneState::CurrentTask::DroneState_CurrentTask_CHARGED) {
                    chargingCount++;

                    if(interComm->droneStates[myid]->curren_task() == DroneState::CurrentTask::DroneState_CurrentTask_CHARGING//yo tambien estoy cargando
                            && it->second->pad_in_use() == pads[closestPad]->getId()//En el mismo charging pad
                            && (it->second->battery_level() < interComm->droneStates[myid]->battery_level()//El tiene menos bateria que yo
                                || (it->second->battery_level() == interComm->droneStates[myid]->battery_level() && it->first < myid)//O tiene igual bateria pero menor id
                               )){

                            shouldLeaveCharge = true;

                    } else {

                        //tengo que cubrir a este drone?
                        if(paths.find(it->first) != paths.end() && it->second->curren_task() != DroneState::CurrentTask::DroneState_CurrentTask_CHARGED){
                            actualPath = it->first;
                            pathSize = paths[it->first].GetPoints().size();
                            interComm->droneStates[myid]->set_covered_drone_id(it->first);
                        }
                    }


                }
            }
        }

        //Estoy en mi path?
        bool inPath = previousMarker != -1  && nextMarker != -1 && previousMarker < pathSize && nextMarker < pathSize
                        && previousPosition.val[0] == paths[myid].GetPoints().at(previousMarker).position.val[0]
                        && previousPosition.val[1] == paths[myid].GetPoints().at(previousMarker).position.val[1]
                        && previousPosition.val[2] == paths[myid].GetPoints().at(previousMarker).position.val[2]
                        && nextPosition.val[0] == paths[myid].GetPoints().at(nextMarker).position.val[0]
                        && nextPosition.val[1] == paths[myid].GetPoints().at(nextMarker).position.val[1]
                        && nextPosition.val[2] == paths[myid].GetPoints().at(nextMarker).position.val[2];

        //Tengo que volver a mi path? ya volvì a mi path?
        if(actualPath != myid && (interComm->droneStates.find(actualPath) == interComm->droneStates.end() || interComm->droneStates[actualPath]->curren_task() == DroneState::CurrentTask::DroneState_CurrentTask_CHARGED)){
            //Vuelvo a mi path
            actualPath = myid;
            pathSize = paths[myid].GetPoints().size();
        } else if(interComm->droneStates[myid]->covered_drone_id() != 0
                  && (interComm->droneStates.find(actualPath) == interComm->droneStates.end() || interComm->droneStates[interComm->droneStates[myid]->covered_drone_id()]->curren_task() == DroneState::CurrentTask::DroneState_CurrentTask_CHARGED)
                  && inPath) {
            //Volvì a mi ruta
            interComm->droneStates[myid]->set_covered_drone_id(0);
        }

        //Si tengo que cargar, no lo estoy haciendo y no hay nadie cargando voy a cargar
        if (interComm->droneStates[myid]->curren_task() != DroneState::CurrentTask::DroneState_CurrentTask_CHARGING
                    && chargingCount < pads.size() && shouldCharge){

            //Paso a charging
            interComm->droneStates[myid]->set_curren_task(DroneState::CurrentTask::DroneState_CurrentTask_CHARGING);

            //Calcular pad mas cercano
            closestPad = -1;
            for (int i = 0 ; i != pads.size(); ++i) {

                if(closestPad == -1){
                    closestPad = i;
                }else {
                    float currentNorm = std::sqrt((pads[i]->getPosition().val[0]-interComm->droneStates[myid]->position().x())
                                                     *(pads[i]->getPosition().val[0]-interComm->droneStates[myid]->position().x())
                                                  + (pads[i]->getPosition().val[1]-interComm->droneStates[myid]->position().y())
                                                     *(pads[i]->getPosition().val[1]-interComm->droneStates[myid]->position().y()));
                    float bestNorm = std::sqrt((pads[closestPad]->getPosition().val[0]-interComm->droneStates[myid]->position().x())
                                                 *(pads[closestPad]->getPosition().val[0]-interComm->droneStates[myid]->position().x())
                                               + (pads[closestPad]->getPosition().val[1]-interComm->droneStates[myid]->position().y())
                                                 * (pads[closestPad]->getPosition().val[1]-interComm->droneStates[myid]->position().y()));
                    std::cout << "CurrentNorm: " << currentNorm << "\n";
                    std::cout << "BestNorm: " << bestNorm << "\n";
                    if(currentNorm < bestNorm){
                        closestPad = i;
                    }
                }
            }
            //Aviso el pad en el que voy a cargar
            interComm->droneStates[myid]->set_pad_in_use(pads[closestPad]->getId());

            //Actualizo lapso y start time
            taskLapse = chargeLapse;
            taskStartTime = runningTime;

        //Si la batteria està llena estoy cargado
        } else if(interComm->droneStates[myid]->battery_level() == 100 && interComm->droneStates[myid]->curren_task() == DroneState::CurrentTask::DroneState_CurrentTask_CHARGING) {

            interComm->droneStates[myid]->set_curren_task(DroneState::CurrentTask::DroneState_CurrentTask_CHARGED);

        //Si termine de cargar y mi path está libre
        }else if(interComm->droneStates[myid]->curren_task() == DroneState::CurrentTask::DroneState_CurrentTask_CHARGED && !someoneCovering) {

            //Reseteo simulador de bateria
            startBattryTime = runningTime;

            //Paso a patruyar
            interComm->droneStates[myid]->set_curren_task(DroneState::CurrentTask::DroneState_CurrentTask_PATROLING);

            //Actualizo lapso y start time
            taskLapse = (rand() % range + 3) * 1000 * 1000;
            taskStartTime = runningTime;

    //Si hay algien sigiendo, estoy en mi path y yo estoy alerta o patruyando cambio a alerta
    } else if(someoneFollowing && inPath &&
       (interComm->droneStates[myid]->curren_task() == DroneState::CurrentTask::DroneState_CurrentTask_ALERT
        || interComm->droneStates[myid]->curren_task() == DroneState::CurrentTask::DroneState_CurrentTask_PATROLING)) {

        interComm->droneStates[myid]->set_curren_task(DroneState::CurrentTask::DroneState_CurrentTask_ALERT);

    } else if(interComm->droneStates[myid]->curren_task() == DroneState::CurrentTask::DroneState_CurrentTask_ALERT//estoy alerta y tengo que volver a patrullar
                    ||  interComm->droneStates[myid]->curren_task() == DroneState::CurrentTask::DroneState_CurrentTask_INNACTIVE//estoy inactivo
                    || (shouldLeaveCharge && interComm->droneStates[myid]->curren_task() == DroneState::CurrentTask::DroneState_CurrentTask_CHARGING)//Hay alguien mas cargando con màs prioridad
                    || (runningTime - taskStartTime > taskLapse && interComm->droneStates[myid]->curren_task() == DroneState::CurrentTask::DroneState_CurrentTask_FOLLOWING)//Termine de seguir a alguien
                ) {

            //Paso a patruyar
            interComm->droneStates[myid]->set_curren_task(DroneState::CurrentTask::DroneState_CurrentTask_PATROLING);

            //Actualizo lapso y start time
            taskLapse = (rand() % range + 3) * 1000 * 1000;
            taskStartTime = runningTime;

        } else if (runningTime - taskStartTime > taskLapse && interComm->droneStates[myid]->curren_task() == DroneState::CurrentTask::DroneState_CurrentTask_PATROLING){//Si termine mi lapso de patrullaje

            //Decido aleatroriamente paso a following o sigo patrullando
            if(rand() % 5 == 1 && inPath) {
                //Paso a following
                interComm->droneStates[myid]->set_curren_task(DroneState::CurrentTask::DroneState_CurrentTask_FOLLOWING);
            }

            //Actualizo lapso y start time
            taskLapse = (rand() % range + 3) * 1000 * 1000;
            taskStartTime = runningTime;
        }


        //Movimiento simulado
        if(runningTime - lastChange > lapseToChange) {

            if (interComm->droneStates[myid]->curren_task() == DroneState::CurrentTask::DroneState_CurrentTask_CHARGING
                    || interComm->droneStates[myid]->curren_task() == DroneState::CurrentTask::DroneState_CurrentTask_CHARGED) {

                previousMarker = nextMarker;
                previousPosition = nextPosition;
                previousRotation = nextRotation;
                nextMarker = -1;
                nextPosition.val[0] = pads[closestPad]->getPosition().val[0];
                nextPosition.val[1] = pads[closestPad]->getPosition().val[1];
                nextPosition.val[2] = pads[closestPad]->getPosition().val[2];
                nextRotation = pads[closestPad]->getRotation().val[2];


            }else if (interComm->droneStates[myid]->curren_task() == DroneState::CurrentTask::DroneState_CurrentTask_INNACTIVE){

                nextMarker = -1;
                nextPosition.val[0] = d->getPosition().val[0];
                nextPosition.val[1] = d->getPosition().val[1];
                nextPosition.val[2] = d->getPosition().val[2];
                nextRotation = d->getRotation().val[2];
                previousMarker = -1;
                previousPosition.val[0] = nextPosition.val[0];
                previousPosition.val[1] = nextPosition.val[1];
                previousPosition.val[2] = nextPosition.val[2];
                previousRotation = d->getRotation().val[2];

            }else if (interComm->droneStates[myid]->curren_task() == DroneState::CurrentTask::DroneState_CurrentTask_ALERT){

                previousMarker = nextMarker;
                previousPosition = nextPosition;
                previousRotation = nextRotation;

                //Mirar hacia el objeto seguido
                cv::Vec3d aux(nextPosition.val[0], nextPosition.val[1], nextPosition.val[2]);
                cv::Vec3d aux2(interComm->droneStates[alertid]->position().x(), interComm->droneStates[alertid]->position().y(), interComm->droneStates[alertid]->position().z());
                aux = aux2 - aux;
                if(aux.val[0] != 0){
                    double edge = atan2 (aux.val[0],aux.val[1]) * 180 / M_PI;
                    nextRotation = edge;
                }

            /*}else if (interComm->droneStates[myid]->curren_task() == DroneState::CurrentTask::DroneState_CurrentTask_FOLLOWING){

                previousMarker = nextMarker;
                previousPosition = nextPosition;
                previousRotation = nextRotation;*/


            } else if (interComm->droneStates[myid]->curren_task() == DroneState::CurrentTask::DroneState_CurrentTask_FOLLOWING
                        || interComm->droneStates[myid]->curren_task() == DroneState::CurrentTask::DroneState_CurrentTask_PATROLING){

                previousMarker = nextMarker;
                previousPosition = nextPosition;
                previousRotation = nextRotation;
                nextMarker = (nextMarker + 1) % pathSize;

                nextPosition.val[0] = paths[actualPath].GetPoints().at(nextMarker).position.val[0];
                nextPosition.val[1] = paths[actualPath].GetPoints().at(nextMarker).position.val[1];
                nextPosition.val[2] = paths[actualPath].GetPoints().at(nextMarker).position.val[2];
                nextRotation = paths[actualPath].GetPoints().at(nextMarker).rotation;
            }

            float distance = std::sqrt((previousPosition.val[0]-nextPosition.val[0])*(previousPosition.val[0]-nextPosition.val[0])
                                          + (previousPosition.val[1]-nextPosition.val[1])*(previousPosition.val[1]-nextPosition.val[1]));
            lapseToChange = (1/speedMS) * (1/distance) * 1000.0 * 1000.0;
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

            //Si estoy following seteo objeto seguido igual a mi position
            if(interComm->droneStates[myid]->curren_task() == DroneState::CurrentTask::DroneState_CurrentTask_FOLLOWING){
                DroneState_Point *fp = new DroneState_Point();
                fp->set_x(position->val[0]);
                fp->set_y(position->val[1]);
                fp->set_z(position->val[2]);
                interComm->droneStates[myid]->set_allocated_followed_position(fp);
            }

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
}

void Brain::cleanup() {

}