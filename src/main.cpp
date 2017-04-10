#include <iostream>
#include <boost/program_options.hpp>
#include <wait.h>
#include "logging/Logger.h"
#include "Body.h"
#include "Brain.h"

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
            ("body", "Starts the body");

    po::variables_map vm;
    po::store(po::parse_command_line(argc, args, desc), vm);
    po::notify(vm);

    bool startBrain = vm.count("brain") > 0;
    bool startBody = vm.count("brain") > 0;

    if(!startBody && !startBrain) {
        std::cout << desc << "\n";
        return 0;
    }

    if(startBody && startBrain) {
        pid_t pid = fork();

        startBody = pid == 0;
        startBrain = !startBody;
    }

    if(startBody){
        Body body;
        Logger::getInstance().setSource("BODY");
        body.communicate("localhost",11500);
    } else {
        Brain brain;
        Logger::getInstance().setSource("BRAIN");
        brain.communicate(11500);
    }

    waitpid(-1, NULL, 0);

    return 0;
}