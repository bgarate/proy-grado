#include <iostream>
#include <boost/program_options.hpp>
#include <wait.h>
#include <tiff.h>
#include <src/hal/dummyHal/dummyHal.h>
#include "logging/Logger.h"
#include "Body.h"
#include "Brain.h"
#include "hal/vrep/vrephal.cpp"
#include "hal/pb2/pb2hal.cpp"

namespace po = boost::program_options;

void welcome_message() {
    std::cout << "PROYECTO DE GRADO - 2017" << std::endl;
    std::cout << "Inteligencia computacional para la planificación de vuelo de vehículos aéreos no tripulados" << std::endl;
    std::cout << "Santiago Díaz, Bruno Garate, Sergio Nesmachnow, Santiago Iturriaga" << std::endl << std::endl;
}

int main(int argc, const char* args[]) {
    welcome_message();

    po::options_description desc = po::options_description("Opciones posibles");

    desc.add_options()
            ("brain", "Starts the brain")
            ("body", "Starts the body")
            ("brainHost", po::value<std::string>(), "Host where to find the brain")
            ("brainPort", po::value<uint8>(), "Port where to find the brain")
            ("broadcastPort", po::value<uint8>(), "Port to which drones can broadcast an advertisement")
            ("advertisementLapse", po::value<int>(), "Lapse in milliseconds between drone advertisements")
            ("commsPort", po::value<int>(), "Port in which the drone acepts incoming connections from other drones")
            ("hal", po::value<string>(), "Hal to be used (dummy,pb2,vrep)");

    po::variables_map vm;
    po::store(po::parse_command_line(argc, args, desc), vm);
    po::notify(vm);

    bool startBrain = vm.count("brain") > 0;
    bool startBody = vm.count("body") > 0;

    if (!startBody && !startBrain) {
        std::cout << "A body or a brain must at least be initialized" << "\n";
        return 0;
    }

    Config* config = new Config();

    if(vm.count("brainHost") > 0) {
        config->setBrainHost(vm["brainHost"].as<std::string>());
    }

    if(vm.count("brainPort") > 0) {
        config->setBrainPort(vm["brainPort"].as<uint8>());
    }

    if(vm.count("commsPort") > 0) {
        config->setCommsPort(vm["commsPort"].as<uint8>());
    }

    if(vm.count("broadcastPort") > 0) {
        config->setAdvertisementLapse(vm["broadcastPort"].as<int>());
    }

    if(vm.count("advertisementLapse") > 0) {
        config->setAdvertisementLapse(vm["advertisementLapse"].as<int>());
    }

    Hal* hal;

    if(startBody) {

        if(vm.count("hal") == 0){
            std::cout << "A Hal type must be chosen: dummy, pb2, vrep" << endl;
            return 0;
        }
        string choosenHal = vm["hal"].as<string>();

        if(choosenHal == "dummy") {
            config->setHalType(HalType::Dummy);
            hal = new DummyHal();
        } else if(choosenHal == "pb2") {
            config->setHalType(HalType::Pb2);
            hal = new Pb2hal();
        } else if(choosenHal == "vrep") {
            config->setHalType(HalType::Vrep);
            hal = new Vrephal();
        } else {
            std::cout << "Unknown Hal type '" + choosenHal + "'" << endl;
            return 0;
        }
    }

    if (startBody && startBrain) {
        pid_t pid = fork();

        startBody = pid == 0;
        startBrain = !startBody;
    }

    if (startBody) {

        Body body(hal);

        body.setup(config);
        body.loop();
        delete hal;

    } else {
        Brain brain;

        brain.setup(config);
        brain.loop();
    }

    delete config;

    Logger::logWarning("Program exiting");
    waitpid(-1, NULL, 0);


    return 0;
}

