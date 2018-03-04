//
// Created by bruno on 25/02/18.
//



#ifndef PROY_GRADO_SHAREDMEMORY_H
#define PROY_GRADO_SHAREDMEMORY_H

#include <src/navigation/MarkerFollower.h>
#include "../config/Config.h"

class StatusInfo {
public:
    cv::Vec3d CurrentPosition;
    cv::Vec3d CurrentPose;
    cv::Vec3d PredictedFuturePosition;
    cv::Vec3d ProjectedPositionOnPath;
    cv::Vec3d TargetOnPath;

    int CurrentTargetId;

    NavigationCommand FollowPathCommand;

    NavigationCommand ExecutedCommand;
};

class SharedMemory {
public:

    StatusInfo getStatusInfo();
    void setStatusInfo(StatusInfo status);

private:

    StatusInfo statusInfo;
    std::mutex statusInfoMutex;

};


#endif //PROY_GRADO_SHAREDMEMORY_H
