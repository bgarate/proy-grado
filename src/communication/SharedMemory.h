//
// Created by bruno on 25/02/18.
//



#ifndef PROY_GRADO_SHAREDMEMORY_H
#define PROY_GRADO_SHAREDMEMORY_H

#include <src/navigation/PathFollower.h>
#include <src/tracking/Follower.h>
#include <src/landtracking/MarkerLand.h>
#include "../config/Config.h"

class BodyInfo {
public:
    /**
     * Indica si está listo para ser
     */
    bool ready = false;
    /**
     * Vector de tres componentes que indica cual es la posición actual
     */
    cv::Vec3d CurrentPosition;
    /**
     * Vector de tres componentes que indica cual es la rotación actual
     */
    cv::Vec3d CurrentPose;
    /**
     * Vector de tres componentes que indica la posición estimada futura
     */
    cv::Vec3d PredictedFuturePosition;
    /*
     * Vector de tres componentes que indica la posición estimada en el path
     */
    cv::Vec3d ProjectedPositionOnPath;
    /**
     * Vector de tres componentes que indica cual es la posición objetivo proyectada en el path
     */
    cv::Vec3d TargetOnPath;

    /**
     * Indica si el drone esta cerca del proximo punto del path objetivo
     */
    bool isShutingDown = false;

    /**
     * Punto del path que es el objetvo actual
     */
    int CurrentTargetId;

    /**
     * Último comando ejectuado al seguir el path
     */
    NavigationCommand FollowPathCommand;
    /**
     * Último comando ejecutado al seguir a un instruso
     */
    NavigationCommand FollowDetectionCommand;
    /**
     * Último comando ejecutado al aterrizar sobre la plataforma
     */
    LandMoveCommand PadLandingCommand;

    /**
     * Último comando ejecutado
     */
    NavigationCommand ExecutedCommand;

    /**
     * Indica la precencia de un intruso
     */
    bool intruderDetected = false;
    /**
     * Indica la posición estimada del instruso
     */
    cv::Vec3d intruderPosition;

    /**
     * Indica si el drone está patrullando sobre el path
     */
    bool inPath = false;
    /**
     * Indica si el drone esta aterrizado en la plataforma de carga
     */
    bool landedInPad = false;
    /**
     * Indica el nivel de batería
     */
    int batteryLevel;

};

class BrainInfo {
public:

    /**
     * Enum con los valores posibles para currentTask
     */
    enum CurrentTask {
        INNACTIVE,
        PATROLING,
        FOLLOWING,
        ALERT,
        CHARGING,
        CHARGED,
        BACKFROMPAD,
        GOINGTOPAD,
        SHUTDOWN
    };

    /**
     * Indica la taréa actual a realizar indicada por el brain
     */
    CurrentTask currentTask;
    /**
     * Indica el id del path que se está siguiendo
     */
    int currentPathId;
    /*
     * Indica el id del pad utilizado para recargar la batería
     */
    int currentPadId;

    /**
     * Posición del intruso detectado por otro dron
     */
    cv::Vec3d alertedPosition;


};

class SharedMemory {
public:

    /**
     * Permite obtener la información del body
     *
     * @return clase del tipo BodyInfo con información del body
     */
    BodyInfo getBodyInfo();
    /**
     * Setea la información del body
     * @param status clase del tipo BodyInfo con información del body
     */
    void setBodyInfo(BodyInfo status);

    /**
     * Permite obtener la información del brain
     *
     * @return clase del tipo BrainInfo con información del brain
     */
    BrainInfo getBrainInfo();
    /**
     * Setea la información del brain
     * @param status clase del tipo BrainInfo con información del brain
     */
    void setBrainInfo(BrainInfo status);

private:

    BodyInfo bodyInfo;
    std::mutex bodyInfoMutex;

    BrainInfo brainInfo;
    std::mutex brainInfoMutex;


};


#endif //PROY_GRADO_SHAREDMEMORY_H
