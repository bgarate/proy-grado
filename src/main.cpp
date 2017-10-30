#include <iostream>
#include <wait.h>
#include <tiff.h>
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
    bool startBody = config->Get(ConfigKeys::Brain::Start);

    if (!startBody && !startBrain) {
        std::cout << "A body or a brain must at least be initialized" << "\n";
        return 0;
    }

    Hal* hal;

    if(startBody) {

        HalType choosenHal = config->Get(ConfigKeys::Body::Hal);

        if(choosenHal == HalType::Dummy) {
            hal = new DummyHal();
        } else if(choosenHal == HalType::Pb2) {
            hal = new Pb2hal();
        } else if(choosenHal == HalType::Vrep) { ;
            hal = new Vrephal();
        } else {
            std::cout << "Unknown Hal type '" << endl;
            return 0;
        }
    }

    bool isParent = true;

    if (startBody && startBrain) {
        pid_t pid = fork();

        isParent = pid == 0;

        startBody = isParent && config->Get(ConfigKeys::Body::ParentOnFork);
        startBrain = !startBody;
    }

    if (startBody) {

        Body body(hal);

        body.setup(config);
        body.loop();
        body.cleanup();
        delete hal;

    } else {
        Brain brain;

        brain.setup(config);
        brain.loop();
        brain.cleanup();
    }

    if(isParent)
        config->Save();

    delete config;

    Logger::logWarning("Program exiting");
    waitpid(-1, NULL, 0);


    return 0;
}

