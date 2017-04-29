#include <iostream>
#include <boost/program_options.hpp>
#include <wait.h>
#include "logging/Logger.h"
#include "Body.h"
#include "Brain.h"
#include "hal/vrep/vrephal.cpp"

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
            ("root", "Acts as root node to which other brains connect to");

    po::variables_map vm;
    po::store(po::parse_command_line(argc, args, desc), vm);
    po::notify(vm);

    bool startBrain = vm.count("brain") > 0;
    bool startBody = vm.count("body") > 0;
    bool isRoot = vm.count("root") > 0;

    if (!startBody && !startBrain) {
        std::cout << desc << "\n";
        return 0;
    }

    if (startBody && startBrain) {
        pid_t pid = fork();

        startBody = pid == 0;
        startBrain = !startBody;
    }

    if (startBody) {
        Hal* hal = new Vrephal();
        Body body(hal);

        body.setup("localhost");
        body.loop();
        delete hal;
    } else {
        Brain brain;

        brain.setup(isRoot);
        brain.loop();
    }

    Logger::logWarning("Program exiting");
    waitpid(-1, NULL, 0);


    return 0;
}

