
#include "SharedMemory.h"

StatusInfo SharedMemory::getStatusInfo() {
    std::lock_guard<std::mutex> lck(statusInfoMutex);
    return statusInfo;
}

void SharedMemory::setStatusInfo(StatusInfo status) {
    std::lock_guard<std::mutex> lck(statusInfoMutex);
    statusInfo = status;
}

ControlInfo SharedMemory::getControlInfo() {
    std::lock_guard<std::mutex> lck(controlInfoMutex);
    return controlInfo;
}

void SharedMemory::setControlInfo(ControlInfo control) {
    std::lock_guard<std::mutex> lck(controlInfoMutex);
    controlInfo = control;
}
