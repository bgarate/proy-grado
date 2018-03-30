//
// Created by bruno on 25/02/18.
//



#ifndef PROY_GRADO_SHAREDMEMORY_H
#define PROY_GRADO_SHAREDMEMORY_H

#include <src/navigation/MarkerFollower.h>
#include <src/tracking/Follower.h>
#include "../config/Config.h"

class BodyInfo {
public:
    bool ready;
        cv::Vec3d CurrentPosition;
    cv::Vec3d CurrentPose;
    cv::Vec3d PredictedFuturePosition;
    cv::Vec3d ProjectedPositionOnPath;
    cv::Vec3d TargetOnPath;

    int CurrentTargetId;

    NavigationCommand FollowPathCommand;
    NavigationCommand FollowDetectionCommand;

    NavigationCommand ExecutedCommand;

    bool intruderDetected;
    cv::Vec3d intruderPosition;

    bool inPath;
    bool landedInPad;
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
