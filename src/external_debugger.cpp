#include <src/debugging/MapDebugger.h>
#include <src/communication/InterComm.h>

namespace chrono = std::chrono;

//Variables
InterComm * interComm;
MapDebugger* mapDebugger;
World world;
Path* path;
long lastRefreshTime = 0;
long pirntLapse = 0.1 * 1000 * 1000;

int main(int argc, const char* args[]) {

    //Init
    Logger::getInstance().setSource("START");

    Config *config = new Config;
    long deltaTime = 0;
    long runningTime = 0;

    if(argc > 1){
        config->SetPath(args[1]);
    } else {
        config->SetPath("config.yaml");
    }

    config->Load();

    interComm = new InterComm();
    interComm->setupInterComm(config, false);

    world = config->GetWorld();
    mapDebugger = new MapDebugger(config, &world);
    mapDebugger->Init();
    //path = config->GetPath();
    path = new Path();

    chrono::steady_clock::time_point startTime = chrono::steady_clock::now();
    chrono::steady_clock::time_point lastTime = startTime;
    chrono::steady_clock::time_point newTime = startTime;

    //Loop
    while (true) {

        lastTime = newTime;
        newTime = chrono::steady_clock::now();

        deltaTime = std::chrono::duration_cast<std::chrono::microseconds>(newTime - lastTime).count();
        runningTime = std::chrono::duration_cast<std::chrono::microseconds>(newTime - startTime).count();

        interComm->interCommStep(runningTime, deltaTime);

        double timeLapse = runningTime - lastRefreshTime;
        if(timeLapse > pirntLapse) {

            mapDebugger->Run(interComm->droneStates,-1,*path, timeLapse);
            lastRefreshTime = runningTime;
        }

    }


}