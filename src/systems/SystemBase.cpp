//
// Created by bruno on 30/03/18.
//

#include "SystemBase.h"


void SystemBase::Init(Config *config, Hal *hal, SharedMemory *shared, VisualDebugger *visualDebugger, NavigationDebugger *navigationDebugger) {

    this->hal = hal;
    this->shared = shared;
    this->visualDebugger = visualDebugger;
    this->config = config;
    this->navigationDebugger = navigationDebugger;

    internalInit();
}

void SystemBase::Update(double deltaTime) {
    brainInfo = shared->getBrainInfo();
    bodyInfo = shared->getBodyInfo();

    internalUpdate(deltaTime);

    shared->setBodyInfo(bodyInfo);
}
