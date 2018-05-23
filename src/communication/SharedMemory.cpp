
#include "SharedMemory.h"

BodyInfo SharedMemory::getBodyInfo() {
    std::lock_guard<std::mutex> lck(bodyInfoMutex);
    return bodyInfo;
}

void SharedMemory::setBodyInfo(BodyInfo status) {
    std::lock_guard<std::mutex> lck(bodyInfoMutex);
    bodyInfo = status;
}

BrainInfo SharedMemory::getBrainInfo() {
    std::lock_guard<std::mutex> lck(brainInfoMutex);
    return brainInfo;
}

void SharedMemory::setBrainInfo(BrainInfo control) {
    std::lock_guard<std::mutex> lck(brainInfoMutex);
    brainInfo = control;
}
