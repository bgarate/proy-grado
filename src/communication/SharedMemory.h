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
    bool ready = false;
    cv::Vec3d CurrentPosition;
    cv::Vec3d CurrentPose;
    cv::Vec3d PredictedFuturePosition;
    cv::Vec3d ProjectedPositionOnPath;
    cv::Vec3d TargetOnPath;

    bool isShutingDown = false;

    int CurrentTargetId;

    NavigationCommand FollowPathCommand;
    NavigationCommand FollowDetectionCommand;
    LandMoveCommand PadLandingCommand;

    NavigationCommand ExecutedCommand;

    bool intruderDetected = false;
    cv::Vec3d intruderPosition;

    bool inPath = false;
    bool landedInPad = false;
    int batteryLevel;

};

class BrainInfo {
public:

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

    CurrentTask currentTask;
    int currentPathId;
    int currentPadId;

    cv::Vec3d alertedPosition;


};

class SharedMemory {
public:

    BodyInfo getBodyInfo();
    void setBodyInfo(BodyInfo status);

    BrainInfo getBrainInfo();
    void setBrainInfo(BrainInfo status);

private:

    BodyInfo bodyInfo;
    std::mutex bodyInfoMutex;

    BrainInfo brainInfo;
    std::mutex brainInfoMutex;


};


#endif //PROY_GRADO_SHAREDMEMORY_H
