//
// Created by bruno on 09/04/17.
//

#include <src/proto/dronestate.pb.h>
#include <src/communication/SharedMemory.h>
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

void Brain::setup(Config* config, SharedMemory* shared) {

    this->config = config;
    this->myid = config->Get(ConfigKeys::Drone::Id);

    this->shared = shared;

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

    //Pads
    paths = config->GetPaths();

    pads = world.getPads();

    //Comienzo inactivo
    interComm->droneStates[myid]->set_current_task(DroneState::CurrentTask::DroneState_CurrentTask_INNACTIVE);
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

    BrainInfo brainInfo = shared->getBrainInfo();
    brainInfo.currentTask = BrainInfo::INNACTIVE;
    brainInfo.currentPathId = myid;
    shared->setBrainInfo(brainInfo);
};

void Brain::loop() {

    //esperar que el body esté listo
    while (!shared->getBodyInfo().ready);

    chrono::steady_clock::time_point startTime = chrono::steady_clock::now();
    chrono::steady_clock::time_point lastTime = startTime;
    chrono::steady_clock::time_point newTime = startTime;

    while (true) {

        //Obtener infos
        BodyInfo bodyInfo = shared->getBodyInfo();
        BrainInfo brainInfo = shared->getBrainInfo();

        lastTime = newTime;
        newTime = chrono::steady_clock::now();

        deltaTime = std::chrono::duration_cast<std::chrono::microseconds>(newTime - lastTime).count();
        runningTime = std::chrono::duration_cast<std::chrono::microseconds>(newTime - startTime).count();

        interComm->interCommStep(runningTime, deltaTime);

        //Atualizar bateria en intercom
        interComm->droneStates[myid]->set_battery_level(bodyInfo.batteryLevel);
        //Actualizar position y rotation en intercom
        DroneState_Point *p = new DroneState_Point();
        p->set_x(bodyInfo.CurrentPosition.val[0]);
        p->set_y(bodyInfo.CurrentPosition.val[1]);
        p->set_z(bodyInfo.CurrentPosition.val[2]);
        interComm->droneStates[myid]->set_allocated_position(p);
        DroneState_Point *r = new DroneState_Point();
        r->set_x(bodyInfo.CurrentPose.val[0]);
        r->set_y(bodyInfo.CurrentPose.val[1]);
        r->set_z(bodyInfo.CurrentPose.val[2]);
        interComm->droneStates[myid]->set_allocated_rotation(r);
        //Si estoy following seteo objeto seguido igual a mi position en intercom
        if(interComm->droneStates[myid]->current_task() == DroneState::CurrentTask::DroneState_CurrentTask_FOLLOWING){
            DroneState_Point *fp = new DroneState_Point();
            fp->set_x(bodyInfo.intruderPosition.val[0]);
            fp->set_y(bodyInfo.intruderPosition.val[1]);
            fp->set_z(bodyInfo.intruderPosition.val[3]);
            interComm->droneStates[myid]->set_allocated_followed_position(fp);
        }

        //Tengo que cargar?
        bool shouldCharge = (bodyInfo.batteryLevel < critialBatteryLevel) || (bodyInfo.batteryLevel < lowBatteryLevel && brainInfo.currentTask == BrainInfo::PATROLING);
        //Hay alguien following? Hay alguien cargando? Hay alguien cargando en mi mismo path con más prioridad? Alguien me está cubriendo?
        bool someoneFollowing = false;
        int chargingCount = 0;
        bool shouldLeaveCharge = 0;
        bool someoneCovering = false;
        for (std::map<int, DroneState*>::iterator it=interComm->droneStates.begin(); it!=interComm->droneStates.end(); ++it) {
            if(it->first != myid){

                //alguien following
                if (it->second->current_task() == DroneState::CurrentTask::DroneState_CurrentTask_FOLLOWING) {
                    someoneFollowing = true;
                    alertid = it->first;
                    brainInfo.alertedPosition.val[0] = it->second->position().x();
                    brainInfo.alertedPosition.val[1] = it->second->position().y();
                    brainInfo.alertedPosition.val[2] = it->second->position().z();
                }
                //alguien me esta cubriendo
                if (it->second->covered_drone_id() == myid) {
                    someoneCovering = true;
                }

                //alguien cargando
                if (it->second->current_task() == DroneState::CurrentTask::DroneState_CurrentTask_GOINGTOPAD
                        || it->second->current_task() == DroneState::CurrentTask::DroneState_CurrentTask_CHARGING
                        || it->second->current_task() == DroneState::CurrentTask::DroneState_CurrentTask_CHARGED
                        || it->second->current_task() == DroneState::CurrentTask::DroneState_CurrentTask_BACKFROMPAD) {
                    //Aumento la cantidad de cargando
                    chargingCount++;

                    //Tengo que dejar de cargar?
                    if(interComm->droneStates[myid]->current_task() == DroneState::CurrentTask::DroneState_CurrentTask_GOINGTOPAD//yo estoy llendo a cargar
                            && it->second->pad_in_use() == pads[closestPad]->getId()//En el mismo charging pad
                            && (it->second->battery_level() < interComm->droneStates[myid]->battery_level()//El tiene menos bateria que yo
                                || (it->second->battery_level() == interComm->droneStates[myid]->battery_level() && it->first < myid)//Ó tiene igual bateria pero menor id
                               )){

                            shouldLeaveCharge = true;
                    }

                    //tengo que cubrir a este drone?
                    if(interComm->droneStates[myid]->covered_drone_id() == 0
                       && it->second->current_task() == DroneState::CurrentTask::DroneState_CurrentTask_CHARGING
                        && paths.find(it->first) != paths.end()){

                        brainInfo.currentPathId = it->first;
                        interComm->droneStates[myid]->set_covered_drone_id(it->first);
                    }
                }
            }
        }



        //Tengo que volver a mi path? ya volvì a mi path?
        if(brainInfo.currentPathId != myid && (interComm->droneStates.find(brainInfo.currentPathId) == interComm->droneStates.end() || interComm->droneStates[brainInfo.currentPathId]->current_task() != DroneState::CurrentTask::DroneState_CurrentTask_CHARGING)){
            //Vuelvo a mi path
            brainInfo.currentPathId = myid;
        } else if(interComm->droneStates[myid]->covered_drone_id() != 0
                  && brainInfo.currentPathId == myid
                  && bodyInfo.inPath) {
            //Volví a mi ruta
            interComm->droneStates[myid]->set_covered_drone_id(0);
        }

        //Si tengo que cargar, no lo estoy haciendo y no hay nadie cargando voy a cargar
        if (shouldCharge
            && brainInfo.currentTask != BrainInfo::CHARGING
            && brainInfo.currentTask != BrainInfo::GOINGTOPAD
            && brainInfo.currentTask != BrainInfo::CHARGED
            && brainInfo.currentTask != BrainInfo::BACKFROMPAD
            && chargingCount < pads.size()) {

            //Paso a going to pad
            interComm->droneStates[myid]->set_current_task(DroneState::CurrentTask::DroneState_CurrentTask_GOINGTOPAD);
            brainInfo.currentTask = BrainInfo::GOINGTOPAD;

            //Calcular pad mas cercano
            closestPad = -1;
            for (int i = 0; i != pads.size(); ++i) {

                if (closestPad == -1) {
                    closestPad = i;
                } else {
                    double currentNorm = std::sqrt(
                            (pads[i]->getPosition().val[0] - bodyInfo.CurrentPosition.val[0])
                            * (pads[i]->getPosition().val[0] - bodyInfo.CurrentPosition.val[0])
                            + (pads[i]->getPosition().val[1] - bodyInfo.CurrentPosition.val[1])
                              * (pads[i]->getPosition().val[1] - bodyInfo.CurrentPosition.val[1]));
                    double bestNorm = std::sqrt(
                            (pads[closestPad]->getPosition().val[0] - bodyInfo.CurrentPosition.val[0])
                            * (pads[closestPad]->getPosition().val[0] - bodyInfo.CurrentPosition.val[0])
                            + (pads[closestPad]->getPosition().val[1] - bodyInfo.CurrentPosition.val[1])
                              * (pads[closestPad]->getPosition().val[1] - bodyInfo.CurrentPosition.val[1]));
                    if (currentNorm < bestNorm) {
                        closestPad = i;
                    }
                }
            }
            //Aviso el pad en el que voy a cargar
            interComm->droneStates[myid]->set_pad_in_use(pads[closestPad]->getId());
            brainInfo.currentPadId = pads[closestPad]->getId();

            //Dejo de cubrir si lo estoy haciendo
            brainInfo.currentPathId = myid;
            interComm->droneStates[myid]->set_covered_drone_id(0);

        //SI estoy going to pad y ya lleguè
        } else if(brainInfo.currentTask == BrainInfo::GOINGTOPAD && bodyInfo.landedInPad){

            //Paso a going to pad
            interComm->droneStates[myid]->set_current_task(DroneState::CurrentTask::DroneState_CurrentTask_CHARGING);
            brainInfo.currentTask = BrainInfo::CHARGING;

        //Si la batteria està llena y estoy cargado
        } else if(bodyInfo.batteryLevel == 100 && brainInfo.currentTask == BrainInfo::CHARGING) {

            interComm->droneStates[myid]->set_current_task(DroneState::CurrentTask::DroneState_CurrentTask_CHARGED);
            brainInfo.currentTask = BrainInfo::CHARGED;

        //Si termine de cargar y mi path está libre
        }else if(brainInfo.currentTask == BrainInfo::CHARGED && !someoneCovering) {

            //Velvo a mi ruta
            interComm->droneStates[myid]->set_current_task(DroneState::CurrentTask::DroneState_CurrentTask_BACKFROMPAD);
            brainInfo.currentTask = BrainInfo::BACKFROMPAD;

        //Si ya volvi a mi ruta despues de cargar
        }else if(bodyInfo.inPath && brainInfo.currentTask == BrainInfo::BACKFROMPAD) {

            //Velvo a patrullar
            interComm->droneStates[myid]->set_current_task(DroneState::CurrentTask::DroneState_CurrentTask_PATROLING);
            brainInfo.currentTask = BrainInfo::PATROLING;


        //Si encontré un intruso
        } else if(bodyInfo.intruderDetected && brainInfo.currentTask == BrainInfo::PATROLING && bodyInfo.inPath){

            interComm->droneStates[myid]->set_current_task(DroneState::CurrentTask::DroneState_CurrentTask_FOLLOWING);
            brainInfo.currentTask = BrainInfo::FOLLOWING;

            DroneState_Point *fp = new DroneState_Point();
            fp->set_x(bodyInfo.intruderPosition.val[0]);
            fp->set_y(bodyInfo.intruderPosition.val[1]);
            fp->set_z(bodyInfo.intruderPosition.val[3]);
            interComm->droneStates[myid]->set_allocated_followed_position(fp);

        //Si hay algien sigiendo, estoy en mi path y yo estoy alerta o patruyando cambio a alerta
        } else if(someoneFollowing && bodyInfo.inPath
                  && (brainInfo.currentTask == BrainInfo::ALERT
                        || brainInfo.currentTask == BrainInfo::PATROLING)) {

            interComm->droneStates[myid]->set_current_task(DroneState::CurrentTask::DroneState_CurrentTask_ALERT);
            brainInfo.currentTask = BrainInfo::ALERT;

        //Hay alguien mas cargando con más prioridad
        //estoy alerta y tengo que volver a patrullar
        //estoy inactivo
        //Termine de seguir a alguien
        } else if(shouldLeaveCharge
                    || brainInfo.currentTask == BrainInfo::ALERT
                    || brainInfo.currentTask == BrainInfo::INNACTIVE
                    || (!bodyInfo.intruderDetected && brainInfo.currentTask == BrainInfo::FOLLOWING)
                ) {

            //Paso a patruyar
            interComm->droneStates[myid]->set_current_task(DroneState::CurrentTask::DroneState_CurrentTask_PATROLING);
            brainInfo.currentTask = BrainInfo::PATROLING;

        }

        //Actualizar map debugger
        if(runningTime - lastRefreshTime > pirntLapse) {

            if(mapEnabled)
                mapDebugger->Run(interComm->droneStates,myid,paths[brainInfo.currentPathId]);

            lastRefreshTime = runningTime;
        }

        //grabar info actualizada
        shared->setBrainInfo(brainInfo);

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