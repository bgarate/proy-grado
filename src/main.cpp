#include <iostream>
#include <wait.h>
#include <tiff.h>
#include <src/communication/SharedMemory.h>
#include "hal/dummyHal/dummyHal.h"
#include "logging/Logger.h"
#include "Body.h"
#include "Brain.h"
#include "hal/vrep/vrephal.cpp"
#include "hal/pb2/pb2hal.cpp"
#include "hal/HalType.hpp"
#include "config/ConfigKeys.h"

void welcome_message() {
    std::cout << "PROYECTO DE GRADO - 2017" << std::endl;
    std::cout << "Inteligencia computacional para la planificación de vuelo de vehículos aéreos no tripulados" << std::endl;
    std::cout << "Santiago Díaz, Bruno Garate, Sergio Nesmachnow, Santiago Iturriaga" << std::endl << std::endl;
}

void runBody(Config* config, SharedMemory* shared) {

    Logger::logDebug("Initializing body");

    Hal* hal;

    HalType choosenHal = config->Get(ConfigKeys::Body::Hal);

    if(choosenHal == HalType::Dummy) {
        hal = new DummyHal();
    } else if(choosenHal == HalType::Pb2) {
        hal = new Pb2hal();
    } else if(choosenHal == HalType::Vrep) { ;
        hal = new Vrephal();
    } else {
        Logger::logAndThrow("Unknown Hal type");
        return;
    }

    Body body(hal);

    body.setup(config);
    body.loop();
    body.cleanup();
    delete hal;
}

void runBrain(Config* config, SharedMemory* shared){
    Logger::logDebug("Initializing brain");
    Brain brain;

    brain.setup(config);
    brain.loop();
    brain.cleanup();
}

int main(int argc, const char* args[]) {
    welcome_message();

    Logger::getInstance().setSource("START");

    Config *config = new Config;

    if(argc > 1){
        config->SetPath(args[1]);
    } else {
        config->SetPath("config.yaml");
    }

    config->Load();

    bool startBrain = config->Get(ConfigKeys::Brain::Start);
    bool startBody = config->Get(ConfigKeys::Body::Start);

    if (!startBody && !startBrain) {
        Logger::logAndThrow("A body or a brain must at least be initialized");
        return 0;
    }

    std::thread brainThread;
    std::thread bodyThread;

    if (startBody)
        bodyThread = std::thread(runBody, config);

    if(startBrain)
        brainThread = std::thread(runBrain, config);

    if(startBody) {
        if(!bodyThread.joinable())
            Logger::logAndThrow("Body thread is not joinable!");
        bodyThread.join();
    }

    if(startBrain) {
        if(!brainThread.joinable())
            Logger::logAndThrow("Brain thread is not joinable!");
        brainThread.join();
    }

    config->Save();

    delete config;

    Logger::logWarning("Program exiting");

    return 0;
}

