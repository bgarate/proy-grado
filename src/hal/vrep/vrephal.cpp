#include "../hal.hpp"
extern "C" {
#include "extApi.h"
}
#include <math.h>
#include <iostream>
#include <string>
#include <algorithm>
#include <iostream>
//opencv
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include "memory"
#include <src/hal/Point.h>

#include <thread>
#include <unistd.h>
#include <atomic>
#include <src/logging/Logger.h>

using namespace std;

class Vrephal: public Hal {

    //Constantes
    const int PORT = 19997;
    const char * HOST = "127.0.0.1";
    const int width = 680;
    const int height = 420;

    //Variables aux
    int clientID;
    simxInt targetHandler, quadricopterHandler, quadricopterFloorCamHandler, quadricopterFrontCamHandler;
    thread* t;
    atomic<int> roll,pitch,yaw,gaz;
    atomic<double> dx, dy,dz, dh;
    atomic<bool> rmoveactive, rmoving, moving, moving1;
    atomic<State> state;

    Config* config;

    std::shared_ptr<cv::Mat> cachedframe;
    chrono::steady_clock::time_point frameTime;
    simxUChar *image = NULL;

    void deamon(){
        while(1){
            usleep(25000);
            simxFloat factor = 0.001;

            //Movimiento relativo
            if(rmoveactive && !rmoving){

                if(dx!=0 ){//pitch
                    rmoving = true;
                    moving=true;
                    if(dx<0){pitch=-50;}else{pitch=50;};
                    this->state = State::Flying;
                }
                if(dy!=0 ){//roll
                    rmoving = true;
                    moving=true;
                    if(dy<0){roll=-50;}else{roll=50;};
                    this->state = State::Flying;
                }
                if(dz!=0 ){//gaz
                    rmoving = true;
                    moving=true;
                    if(dz<0){gaz=-50;}else{gaz=50;};
                    if(this->state != State::Landing){
                        this->state = State::Flying;
                    } else {
                        gaz = -100;
                    }

                }
                if(dh!=0 ){//yaw
                    rmoving = true;
                    moving=true;
                    if(dh<0){yaw=-50;}else{yaw=50;};
                    this->state = State::Flying;
                }
            } else if (rmoveactive) {//controlar el fin del movminento

                if(abs(dx) <0.1){//pitch
                    pitch=0;
                }
                if(abs(dy) <0.1){//roll
                    roll=0;
                }
                if(abs(dz) <0.1){//gaz
                    gaz=0;
                }
                if(abs(dh) <0.1){//yaw
                    yaw=0;
                }

                if(pitch==0 && yaw==0 && gaz==0 && roll==0){
                    this->moving=true;

                    this->dx = 0;//pitch
                    this->dy = 0;//yaw
                    this->dz = 0;//gaz
                    this->dh = 0;//roll
                    this->rmoving=false;
                    this->rmoveactive=false;


                    if(this->state != State::Landing){
                        this->state = State::Hovering;
                    }
                }
            }


            if(!this->moving && this->moving1){

                //normalize
                if(this->roll<-100){this->roll=0;}else if(this->roll>100){this->roll=0;}
                if(this->pitch<-100){this->pitch=0;}else if(this->pitch>100){this->pitch=0;}
                if(this->yaw<-100){this->yaw=-0;}else if(this->yaw>100){this->yaw=0;}
                if(this->gaz<-100){this->gaz=-0;}else if(this->gaz>100){this->gaz=0;}

                simxFloat orientation[3];
                simxFloat position[3];
                simxFloat rfactor = 4;

                position[0] = -rfactor*this->pitch*factor;//pitch
                position[1] = -rfactor*this->roll*factor;//roll
                position[2] = -rfactor*this->gaz*factor;//gaz
                orientation[0]= 0;
                orientation[1]= 0;
                orientation[2]= -rfactor*(this->yaw*factor);//yaw

                simxSetObjectOrientation(clientID, targetHandler, targetHandler, orientation, simx_opmode_blocking);
                simxSetObjectPosition(clientID, targetHandler, targetHandler, position, simx_opmode_blocking);

                usleep(25000);

                position[0] = -position[0]*1;//pitch
                position[1] = -position[1]*1;//roll
                position[2] = 0;//gaz
                orientation[0]= 0;
                orientation[1]= 0;
                orientation[2]= -orientation[2]/2;//yaw

                simxSetObjectPosition(clientID, targetHandler, targetHandler, position, simx_opmode_blocking);

                this->roll=0;
                this->pitch=0;
                this->yaw=0;
                this->gaz=0;
                moving1 = false;

            } else if(this->moving){

                //normalize
                if(this->roll<-100){this->roll=-100;}else if(this->roll>100){this->roll=100;}
                if(this->pitch<-100){this->pitch=-100;}else if(this->pitch>100){this->pitch=100;}
                if(this->yaw<-100){this->yaw=-100;}else if(this->yaw>100){this->yaw=100;}
                if(this->gaz<-100){this->gaz=-100;}else if(this->gaz>100){this->gaz=100;}

                simxFloat orientation[3];
                simxFloat position[3];

                position[0] = (this->pitch*factor);//pitch
                position[1] = (this->roll * factor);//roll
                position[2] = (this->gaz*factor);//gaz
                orientation[0]= 0;
                orientation[1]= 0;
                orientation[2]= (this->yaw*factor);//yaw

                if(rmoving){

                    if(abs(dx) < abs(position[0])){
                        this->dx = 0;
                    }else{
                        this->dx = this->dx - position[0];
                    }

                    if(abs(dy) < abs(position[1])){
                        this->dy = 0;
                    }else{
                        this->dy = this->dy - position[1];
                    }

                    if(abs(dz) < abs(position[2])){
                        this->dz = 0;
                    }else{
                        this->dz = this->dz - position[2];
                    }

                    if(abs(dh) < abs(orientation[2])){
                        this->dh = 0;
                    }else{
                        this->dh = this->dh - orientation[2];
                    }
                }

                simxSetObjectOrientation(clientID, targetHandler, targetHandler, orientation, simx_opmode_blocking);
                simxSetObjectPosition(clientID, targetHandler, targetHandler,  position, simx_opmode_blocking);

                moving1 = true;
            }

            if(state == State::Landing){
                simxFloat position[3];
                simxGetObjectPosition(clientID, quadricopterHandler, -1, position, simx_opmode_blocking);
                if(position[2] < 0.2 && position[2] > 0){
                    simxSetIntegerSignal(clientID,"motorsoff",1,simx_opmode_blocking);
                    this->state = State::Landed;
                }
            }
            /*if(this->state == State::Landed){
                simxFloat position[3];
                simxGetObjectPosition(clientID, quadricopterHandler, -1, position, simx_opmode_blocking);
                if(position[2] < 0.2 && position[2] > 0){
                    simxSetIntegerSignal(clientID,"motorsoff",1,simx_opmode_blocking);
                    this->state = State::Landed;
                }
            }*/

            if(state == State::TakingOff){
                simxFloat position[3];
                simxGetObjectPosition(clientID, quadricopterHandler, -1, position, simx_opmode_blocking);
                if(position[2] > 0.9){
                    this->state = State::Hovering;
                }
            }

        }
    }


public:

    /************Constructor*************/

    void Connect() {
        //Conectar con V-REP
        clientID = simxStart(HOST, PORT, true, true, 5000, 5);
        simxInt res = simxStartSimulation(clientID, simx_opmode_blocking);

        //Obtener handlers
        simxInt res1 = simxGetObjectHandle(clientID, "Quadricopter_target",&targetHandler,simx_opmode_blocking);
        simxInt res2 = simxGetObjectHandle(clientID, "Quadricopter",&quadricopterHandler,simx_opmode_blocking);
        simxInt res3 = simxGetObjectHandle(clientID, "Vision_sensorFloor",&quadricopterFloorCamHandler,simx_opmode_blocking);
        simxInt res4 = simxGetObjectHandle(clientID, "Vision_sensorFront",&quadricopterFrontCamHandler,simx_opmode_blocking);

        //Iniciar deamon
        this->moving=false;
        this->roll=0;
        this->pitch=0;
        this->yaw=0;
        this->gaz=0;

        this->dx = 0;//pitch
        this->dy = 0;//yaw
        this->dz = 0;//gaz
        this->dh = 0;//roll
        this->rmoveactive = false;
        this->rmoving=false;

        this->t = new thread(&Vrephal::deamon, this);

        this->state = State::Landed;
    }

    void Disconnect(){
        simxInt res = simxStopSimulation(clientID, simx_opmode_blocking);

    }

    State getState() {
        return state;
    }

    bool isRmoving() {
        return rmoveactive;
    }

    void setup(Config* config) {
        this->config = config;
    }

    /************Movimiento*************/

    //Set movimientos
    void move(int roll, int pitch, int yaw, int gaz){

        if (state == State::Flying || state == State::Hovering) {

            if(roll!=0 || pitch!=0 || yaw!=0 || gaz!=0){

                this->roll=-roll;
                this->pitch=pitch;
                this->yaw=-yaw;
                this->gaz=gaz;
                this->moving=true;

                this->dx = 0;//pitch
                this->dy = 0;//yaw
                this->dz = 0;//gaz
                this->dh = 0;//roll
                this->rmoveactive = false;
                this->rmoving=false;

                this->state = State::Flying;
            } else {
                this->moving=false;
                this->roll=0;
                this->pitch=0;
                this->yaw=0;
                this->gaz=0;

                this->dx = 0;//pitch
                this->dy = 0;//yaw
                this->dz = 0;//gaz
                this->dh = 0;//roll
                this->rmoveactive = false;
                this->rmoving=false;

                this->state = State::Hovering;
            }
        } else {
            Logger::logWarning("Cannot move: drone isn't flying or hovering");
        }
    }

    void rmove(double dx, double dy, double dz, double dh){

        if (state == State::Flying || state == State::Hovering) {

            this->moving=false;
            this->roll=0;
            this->pitch=0;
            this->yaw=0;
            this->gaz=0;

            this->dx = dx;//pitch
            this->dy = -dy;//yaw
            this->dz = -dz;//gaz
            this->dh = -dh;//roll
            if(dx!=0 || dy !=0 || dz!=0 || dh!=0)
                this->rmoveactive = true;

        } else {
            Logger::logWarning("Cannot rmove: drone isn't flying or hovering");
        }
    }

    // --> Despegue y aterrizaje
    void land(){

        Logger::logInfo("Landing");

        if (state == State::Flying || state == State::Hovering || state == State::TakingOff){
            simxFloat position[3];
            simxGetObjectPosition(clientID, targetHandler, -1, (simxFloat *)position, simx_opmode_blocking);

            //Modificar posicion
            //position[2] = 0.05;
            this->rmove(0,0,position[2],0);

            //simxSetObjectPosition(clientID, targetHandler, -1, position, simx_opmode_oneshot);
            this->state = State::Landing;

        } else {
            Logger::logWarning("Cannot land: drone isn't flying, hovering or taking off");
        }
    }
    void takeoff(){

        Logger::logInfo("Taking off");

        if (state == State::Landed){
            simxFloat position[3];
            simxGetObjectPosition(clientID, targetHandler, -1, position, simx_opmode_blocking);

            //Modificar posicion
            position[2] = 1;

            simxSetIntegerSignal(clientID,"motorsoff",0,simx_opmode_blocking);
            simxSetObjectPosition(clientID, targetHandler, -1, position, simx_opmode_oneshot);

            this->state = State::TakingOff;

        } else {
            Logger::logWarning("Cannot take off: drone isn't landed");
        }
    }

    /************Estado del drone*************/

    // --> Batería
    int bateryLevel(){
        return 100;
    }

    // --> Intensidad de la conexión
    //todo

    /************Cámara*************/

    // --> Obtener captura de imagen (ambas cámaras)
    std::shared_ptr<cv::Mat> getFrame(Camera cam){

        if(std::chrono::duration_cast<std::chrono::microseconds>
                (chrono::steady_clock::now() - this->frameTime).count() > (0.25 * 1000000)) {

            simxInt cameraHandler;
            if (cam == Camera::Front) {
                cameraHandler = quadricopterFrontCamHandler;
            } else {
                cameraHandler = quadricopterFloorCamHandler;
            }

            simxInt resolution[2];
            
            simxInt aux = simxGetVisionSensorImage(clientID, cameraHandler, resolution, &image, 0,
                                                   simx_opmode_blocking);

            //convertir imagen
            cachedframe =
                    std::shared_ptr<cv::Mat>(new cv::Mat(resolution[1], resolution[0], CV_8UC3, image));
            if (cachedframe != NULL && cachedframe->rows > 0 && cachedframe->cols > 0) {
                cv::cvtColor(*cachedframe, *cachedframe, cv::COLOR_BGR2RGB);
                cv::flip(*cachedframe, *cachedframe, 0);
            } else {
                cachedframe = NULL;
            }

            this->frameTime = chrono::steady_clock::now();
        }
        return cachedframe;
    }

    /************Posición*************/

    // --> Altura
    double getAltitude(){

        simxFloat position[3];
        simxGetObjectPosition(clientID, quadricopterHandler, -1, position, simx_opmode_blocking);

        double pos = position[2];
        return pos;
    }

    // --> Orientación
    Point getOrientation(){

        simxFloat orientation[3];
        simxGetObjectOrientation(clientID, quadricopterHandler, -1, orientation, simx_opmode_blocking);

        Point ori;
        ori.x = orientation[0];
        ori.y = orientation[1];
        ori.z = orientation[2];

        return ori;
    }

    // --> Coordenadas
    Point getGPSPosition(){

        simxFloat position[3];
        simxGetObjectPosition(clientID, quadricopterHandler, -1, position, simx_opmode_blocking);

        Point pos;
        pos.x = position[0];
        pos.y = position[1];
        pos.z = position[2];

        return pos;
    }


};