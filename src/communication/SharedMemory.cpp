
#include "SharedMemory.h"

StatusInfo SharedMemory::getStatusInfo() {
    std::lock_guard lck(statusInfoMutex);
    return statusInfo;
}

void SharedMemory::setStatusInfo(StatusInfo status) {
    std::lock_guard lck(statusInfoMutex);
    statusInfo = status;
}
