//
// Created by bruno on 04/03/18.
//

#ifndef PROY_GRADO_VIRTUALDRONESTATE_H
#define PROY_GRADO_VIRTUALDRONESTATE_H


#include <src/debugging/VisualDebugger.h>
#include <src/communication/SharedMemory.h>
#include <src/navigation/NavigationDebugger.h>
#include <src/config/ConfigKeys.h>
#include <chrono>
#include <src/utils/Helpers.h>
#include "IBodyState.h"
#include "StepName.h"
#include "BodyStateBase.h"

class VirtualDroneState: public BodyStateBase {
public:

    std::string getName() override {
        return StepName::VIRTUAL_DRONE;
    }

    void internalInit() override {


        control->getSystemManager()->Disable<FollowerSystem>();
        control->getSystemManager()->Enable<PadLandingSystem>();
        control->getSystemManager()->Disable<MarkerTrackerSystem>();
        control->getSystemManager()->Disable<PadLandingSystem>();

        this->myid = config->Get(ConfigKeys::Drone::Id);

        this->shared = shared;

    }

    void prepare() override {

        //Simulated movement
        paths = config->GetPaths();

        std::vector<WorldObject *> padsaux = config->GetWorld().getPads();
        for (int i = 0; i != padsaux.size(); ++i) {
            pads[padsaux[i]->getId()] = padsaux[i];
        }

        WorldObject* d = config->GetWorld().getDrone(myid);
        nextMarker = -1;
        nextPosition.val[0] = d->getPosition().val[0];
        nextPosition.val[1] = d->getPosition().val[1];
        nextPosition.val[2] = d->getPosition().val[2];
        nextRotation = d->getRotation().val[2];
        previousMarker = -1;
        previousPosition.val[0] = nextPosition.val[0];
        previousPosition.val[1] = nextPosition.val[1];
        previousPosition.val[2] = nextPosition.val[2];
        previousRotation = nextRotation;

        BodyInfo bodyInfo = shared->getBodyInfo();

        bodyInfo.batteryLevel = 100;
        bodyInfo.CurrentPosition.val[0] = d->getPosition().val[0];
        bodyInfo.CurrentPosition.val[1] = d->getPosition().val[1];
        bodyInfo.CurrentPosition.val[2] = d->getPosition().val[2];
        bodyInfo.CurrentPose.val[0] = d->getRotation().val[0];
        bodyInfo.CurrentPose.val[1] = d->getRotation().val[1];
        bodyInfo.CurrentPose.val[2] = d->getRotation().val[2];
        bodyInfo.intruderDetected = false;
        bodyInfo.inPath = false;
        bodyInfo.landedInPad = false;
        bodyInfo.ready = true;

        shared->setBodyInfo(bodyInfo);

        startTime = std::chrono::steady_clock::now();
        lastTime = startTime;
        newTime = startTime;

        //Simulación de bateria
        startBattryTime = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - startTime).count();

        //COMPORAMIENTO SIMULADO VARIABLES
        srand(time(NULL));
        range = 10 - 3 + 1;
        taskLapse = (rand() % range + 3) * 1000 * 1000;
        taskStartTime = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - startTime).count();

    }

    void internalStep(double deltaTime) override {

        lastTime = newTime;
        newTime = std::chrono::steady_clock::now();

        deltaTime = std::chrono::duration_cast<std::chrono::microseconds>(newTime - lastTime).count();
        runningTime = std::chrono::duration_cast<std::chrono::microseconds>(newTime - startTime).count();

        //Actualizar battery level
        //Si estoy cargando aumento el battery level
        if(brainInfo.currentTask == BrainInfo::CHARGING || brainInfo.currentTask == BrainInfo::CHARGED){

            double diff = (double)(runningTime - taskStartTime) / chargeLapse;

            if(diff >= 1)
                bodyInfo.batteryLevel = 100;
            else
                bodyInfo.batteryLevel = (int)(diff *100);

            //Si no estoy cargando bajo el battery level
        } else {

            double diff = (double)(runningTime - startBattryTime) / batteryDuration;
            if(diff >= 1)
                bodyInfo.batteryLevel = 0;
            else
                bodyInfo.batteryLevel = (int)(100 - diff *100);

        }

        //Estoy en mi path?
        bodyInfo.inPath = previousMarker != -1  && nextMarker != -1 && previousMarker < paths[myid].GetPoints().size() && nextMarker < paths[myid].GetPoints().size()
                      && previousPosition.val[0] == paths[myid].GetPoints().at(previousMarker).position.val[0]
                      && previousPosition.val[1] == paths[myid].GetPoints().at(previousMarker).position.val[1]
                      && previousPosition.val[2] == paths[myid].GetPoints().at(previousMarker).position.val[2]
                      && nextPosition.val[0] == paths[myid].GetPoints().at(nextMarker).position.val[0]
                      && nextPosition.val[1] == paths[myid].GetPoints().at(nextMarker).position.val[1]
                      && nextPosition.val[2] == paths[myid].GetPoints().at(nextMarker).position.val[2];

        //Estoy en la plataforma?
        if(brainInfo.currentTask == BrainInfo::GOINGTOPAD && !bodyInfo.landedInPad && nextMarker == previousMarker){

            bodyInfo.landedInPad = true;
            //Actualizo lapso y start time para calculo de bateria
            taskLapse = chargeLapse;
            taskStartTime = runningTime;

        } else if(brainInfo.currentTask == BrainInfo::BACKFROMPAD && bodyInfo.landedInPad) {

            bodyInfo.landedInPad = false;
            //Reseteo simulador de bateria
            startBattryTime = runningTime;

            //Actualizo lapso y start time
            taskLapse = (rand() % range + 3) * 1000 * 1000;
            taskStartTime = runningTime;
        }

        //Instruso simulado
        if (runningTime - taskStartTime > taskLapse && brainInfo.currentTask == BrainInfo::PATROLING){

            //Decido aleatroriamente paso a following o sigo patrullando
            if(rand() % 5 == 1 && bodyInfo.inPath) {

                //Paso a following
                bodyInfo.intruderDetected = true;
            }

            //Actualizo lapso y start time
            taskLapse = (rand() % range + 3) * 1000 * 1000;
            taskStartTime = runningTime;
        } else if (runningTime - taskStartTime > taskLapse && bodyInfo.intruderDetected){

            //dejo following
            bodyInfo.intruderDetected = false;

            //Actualizo lapso y start time
            taskLapse = (rand() % range + 3) * 1000 * 1000;
            taskStartTime = runningTime;
        }

        //Movimiento simulado
        if(runningTime - lastChange > lapseToChange) {

            if (brainInfo.currentTask == BrainInfo::GOINGTOPAD
                || brainInfo.currentTask == BrainInfo::CHARGING
                || brainInfo.currentTask == BrainInfo::CHARGED) {

                previousMarker = nextMarker;
                previousPosition = nextPosition;
                previousRotation = nextRotation;
                nextMarker = -1;
                nextPosition.val[0] = pads[brainInfo.currentPadId]->getPosition().val[0];
                nextPosition.val[1] = pads[brainInfo.currentPadId]->getPosition().val[1];
                nextPosition.val[2] = pads[brainInfo.currentPadId]->getPosition().val[2];
                nextRotation = pads[brainInfo.currentPadId]->getRotation().val[2];


            }else if (brainInfo.currentTask == BrainInfo::INNACTIVE){

                /*nextMarker = -1;
                nextPosition.val[0] = d->getPosition().val[0];
                nextPosition.val[1] = d->getPosition().val[1];
                nextPosition.val[2] = d->getPosition().val[2];
                nextRotation = d->getRotation().val[2];
                previousMarker = -1;
                previousPosition.val[0] = nextPosition.val[0];
                previousPosition.val[1] = nextPosition.val[1];
                previousPosition.val[2] = nextPosition.val[2];
                previousRotation = d->getRotation().val[2];*/

            }else if (brainInfo.currentTask == BrainInfo::ALERT){

                previousMarker = nextMarker;
                previousPosition = nextPosition;
                previousRotation = nextRotation;

                //Mirar hacia el objeto seguido
                cv::Vec3d aux(nextPosition.val[0], nextPosition.val[1], nextPosition.val[2]);
                cv::Vec3d aux2(brainInfo.alertedPosition.val[0], brainInfo.alertedPosition.val[1], brainInfo.alertedPosition.val[2]);
                aux = aux2 - aux;
                if(aux.val[0] != 0){
                    double edge = atan2 (aux.val[0],aux.val[1]) * 180 / M_PI;
                    nextRotation = edge;
                }

            /*}else if (interComm->droneStates[myid]->current_task() == DroneState::CurrentTask::DroneState_CurrentTask_FOLLOWING){

                    previousMarker = nextMarker;
                    previousPosition = nextPosition;
                    previousRotation = nextRotation;*/


            } else if (brainInfo.currentTask == BrainInfo::FOLLOWING
                       || brainInfo.currentTask == BrainInfo::PATROLING
                       || brainInfo.currentTask == BrainInfo::BACKFROMPAD){

                previousMarker = nextMarker;
                previousPosition = nextPosition;
                previousRotation = nextRotation;
                nextMarker = (nextMarker + 1) % paths[brainInfo.currentPathId].GetPoints().size();

                nextPosition.val[0] = paths[brainInfo.currentPathId].GetPoints().at(nextMarker).position.val[0];
                nextPosition.val[1] = paths[brainInfo.currentPathId].GetPoints().at(nextMarker).position.val[1];
                nextPosition.val[2] = paths[brainInfo.currentPathId].GetPoints().at(nextMarker).position.val[2];
                nextRotation = paths[brainInfo.currentPathId].GetPoints().at(nextMarker).rotation;
            }

            double distance = std::sqrt((previousPosition.val[0]-nextPosition.val[0])*(previousPosition.val[0]-nextPosition.val[0])
                                       + (previousPosition.val[1]-nextPosition.val[1])*(previousPosition.val[1]-nextPosition.val[1]));
            if (distance == 0){distance = 1;}
            lapseToChange = (1/speedMS) * distance * 1000.0 * 1000.0;
            lastChange = runningTime;
        }

        //Calcular position y rotation
        double difference = (double)(runningTime - lastChange) / lapseToChange;
        cv::Vec3d *position;
        cv::Vec3d *rotation;
        rotation = new cv::Vec3d(0, 0, 0);

        //Calcular position
        bodyInfo.CurrentPosition.val[0] = (previousPosition.val[0]*(1-difference)
                                           + nextPosition.val[0]*difference);
        bodyInfo.CurrentPosition.val[1] = (previousPosition.val[1]*(1-difference)
                                           + nextPosition.val[1]*difference);
        bodyInfo.CurrentPosition.val[2] = 1;

        bodyInfo.intruderPosition.val[0] = bodyInfo.CurrentPosition.val[0];
        bodyInfo.intruderPosition.val[1] = bodyInfo.CurrentPosition.val[1];
        bodyInfo.intruderPosition.val[2] = bodyInfo.CurrentPosition.val[2];

        //Calcular rotation
        float a = previousRotation;
        float b = nextRotation;
        bodyInfo.CurrentPose.val[0] = 0;
        bodyInfo.CurrentPose.val[1] = 0;
        bodyInfo.CurrentPose.val[2] = a + Helpers::angleDifference(b,a)*difference;

    }

    void leave() override {

    }

    void cleanup() override {

    }
private:
    bool takingOff = false;

    unsigned int myid;

    long deltaTime = 0;
    long runningTime = 0;
    std::chrono::steady_clock::time_point startTime;
    std::chrono::steady_clock::time_point lastTime;
    std::chrono::steady_clock::time_point newTime;

    int range;
    int taskLapse;
    long taskStartTime;

    long startBattryTime;

    //Movimiento simulado
    std::map<int, Path> paths;
    std::map<int, WorldObject *> pads;


    int nextMarker, previousMarker;
    cv::Vec3d previousPosition;
    double previousRotation;
    cv::Vec3d nextPosition;
    double nextRotation;

    long lastChange = 0;
    long lapseToChange; //= 2 * 1000 * 1000;
    float speedMS = 0.5;

    //Bateria simulada
    int batteryDuration = 60 * 1000 * 1000;
    long chargeLapse = 10 * 1000 * 1000;
};


#endif //PROY_GRADO_TAKINGOFFSTATE_H
