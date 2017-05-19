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

#include <thread>
#include <unistd.h>
#include <atomic>

using namespace std;

class Vrephal: public Hal {

	//Constantes
	const int PORT = 19997;
	const char * HOST = "127.0.0.1"; 

	//Variables aux 
	int clientID;
	simxInt targetHandler, quadricopterHandler, quadricopterFloorCamHandler, quadricopterFrontCamHandler;
    thread* t;
    atomic<int> moving, moving1,roll,pitch,yaw,gaz;

	//Funcion auxiliar

	void deamon(){
		while(1){
            usleep(25000);

            simxFloat factor = 0.001;

            if(this->moving==0 && this->moving1==1){

            	//normalize
	            if(this->roll<-100){this->roll=0;}else if(this->roll>100){this->roll=0;}
	            if(this->pitch<-100){this->pitch=0;}else if(this->pitch>100){this->pitch=0;}
	            if(this->yaw<-100){this->yaw=-0;}else if(this->yaw>100){this->yaw=0;}
	            if(this->gaz<-100){this->gaz=-0;}else if(this->gaz>100){this->gaz=0;}

	            simxFloat orientation[3];
	            simxFloat position[3];
	            simxFloat rfactor = 4;

	            position[0] = -rfactor*this->pitch*factor;//pitch
	            position[1] = -rfactor*this->yaw*factor;//yaw
	            position[2] = -rfactor*this->gaz*factor;//gaz
	            orientation[0]= 0;
	            orientation[1]= 0;
	            orientation[2]= -rfactor*(this->roll*factor);//roll

	            simxSetObjectOrientation(clientID, targetHandler, targetHandler, orientation, simx_opmode_blocking);
            	simxSetObjectPosition(clientID, targetHandler, targetHandler, position, simx_opmode_blocking);

            	usleep(25000);

            	position[0] = -position[0]/2;//pitch
	            position[1] = -position[1]/2;//yaw
	            position[2] = 0;//gaz
	            orientation[0]= 0;
	            orientation[1]= 0;
	            orientation[2]= -orientation[2]/2;//roll

            	simxSetObjectPosition(clientID, targetHandler, targetHandler, position, simx_opmode_blocking);

				this->roll=0;
			    this->pitch=0;
			    this->yaw=0;
			    this->gaz=0;
			    moving1 = 0;

            } else if(this->moving==1){

            	//normalize
	            if(this->roll<-100){this->roll=-100;}else if(this->roll>100){this->roll=100;}
	            if(this->pitch<-100){this->pitch=-100;}else if(this->pitch>100){this->pitch=100;}
	            if(this->yaw<-100){this->yaw=-100;}else if(this->yaw>100){this->yaw=100;}
	            if(this->gaz<-100){this->gaz=-100;}else if(this->gaz>100){this->gaz=100;}

	            simxFloat orientation[3];
	            simxFloat position[3];

	            position[0] = (this->pitch*factor);//pitch
	            position[1] = (this->yaw * factor);//yaw
	            position[2] = (this->gaz*factor);//gaz
	            orientation[0]= 0;
	            orientation[1]= 0;
	            orientation[2]= (this->roll*factor);//roll

            	simxSetObjectOrientation(clientID, targetHandler, targetHandler, orientation, simx_opmode_blocking);
            	simxSetObjectPosition(clientID, targetHandler, targetHandler,  position, simx_opmode_blocking);

            	moving1 = 1;
            }

            
		}
	}


	public:

	/************Constructor*************/ 

	Vrephal(){
		//Conectar con V-REP
		clientID = simxStart(HOST, PORT, true, true, 5000, 5); 
		
	    //Obtener handlers
	    simxInt res1 = simxGetObjectHandle(clientID, "Quadricopter_target",&targetHandler,simx_opmode_blocking);
	    simxInt res2 = simxGetObjectHandle(clientID, "Quadricopter",&quadricopterHandler,simx_opmode_blocking);
	    simxInt res3 = simxGetObjectHandle(clientID, "Vision_sensorFloor",&quadricopterFloorCamHandler,simx_opmode_blocking);
	    simxInt res4 = simxGetObjectHandle(clientID, "Vision_sensorFront",&quadricopterFrontCamHandler,simx_opmode_blocking);

		//Iniciar deamon
		this->moving=0;
		this->roll=0;
	    this->pitch=0;
	    this->yaw=0;
	    this->gaz=0;
		this->t = new thread(&Vrephal::deamon, this);
	}


	void Connect() {

	}

	void Disconnect(){

	}

	/************Movimiento*************/

	//Set movimientos
	void move(int roll, int pitch, int yaw, int gaz){

		if(roll!=0 || pitch!=0 || yaw!=0 || gaz!=0){
			this->moving=1;
			this->roll=roll;
		    this->pitch=pitch;
		    this->yaw=yaw;
		    this->gaz=gaz;
		} else {
			this->moving=0;
		}
	}


	/*// --> Rotación horizontal
	void hrotate(double vel){

		simxFloat * orientation = new simxFloat[3];
	  	simxGetObjectOrientation(clientID, targetHandler, -1, orientation, simx_opmode_blocking);

	  	//Calcular orientacion	  	
        orientation[0]= orientation[0];
        orientation[1]= orientation[1];
        orientation[2]= orientation[2] + (vel*0.1);

		simxSetObjectOrientation(clientID, targetHandler, -1, orientation, simx_opmode_blocking);

	}

	// --> Movimiento horizontal
	void hmove(double angle, double vel){

		//calcular vector
		double x = vel*cos(angle*2*M_PI/360.0);
		double y = vel*sin(angle*2*M_PI/360.0);

		simxFloat * position = new simxFloat[3];
		simxFloat * newposition = new simxFloat[3];
		simxGetObjectPosition(clientID, targetHandler, -1, position, simx_opmode_blocking);

	    //Modificar posicion
	    newposition[0] = position[0] + x*0.1;
	    newposition[1] = position[1] + y*0.1;
	    newposition[2] = position[2];

	    simxSetObjectPosition(clientID, targetHandler, -1, newposition, simx_opmode_blocking);
	    simxSetObjectPosition(clientID, targetHandler, -1, position, simx_opmode_blocking);
	    simxSetObjectPosition(clientID, targetHandler, -1, newposition, simx_opmode_blocking);
	}

	// --> Movimiento vertical
	void vmove(double vel){

		simxFloat * position = new simxFloat[3];
		simxFloat * newposition = new simxFloat[3];
		simxGetObjectPosition(clientID, targetHandler, -1, position, simx_opmode_blocking);

	    //Modificar posicion
	    newposition[0] = position[0];
	    newposition[1] = position[1];
	    newposition[2] = position[2] + vel*0.1;

	    simxSetObjectPosition(clientID, targetHandler, -1, newposition, simx_opmode_blocking);
	    simxSetObjectPosition(clientID, targetHandler, -1, position, simx_opmode_blocking);
	    simxSetObjectPosition(clientID, targetHandler, -1, newposition, simx_opmode_blocking);

	}*/

	// --> Despegue y aterrizaje
	void land(){

		simxFloat position[3];
		simxGetObjectPosition(clientID, targetHandler, -1, (simxFloat *)position, simx_opmode_blocking);

	    //Modificar posicion
	    position[2] = 0.05;

	    simxSetObjectPosition(clientID, targetHandler, -1, position, simx_opmode_oneshot);

	   	while(1){
			simxGetObjectPosition(clientID, quadricopterHandler, -1, position, simx_opmode_blocking);
			if(position[2]< 0.1){
				break;
			}
	    }

	    simxSetIntegerSignal(clientID,"motorsoff",1,simx_opmode_blocking);

	}
	void takeoff(){

		simxFloat position[3];
		simxGetObjectPosition(clientID, targetHandler, -1, position, simx_opmode_blocking);

	    //Modificar posicion
	    position[2] = 1;

	    simxSetIntegerSignal(clientID,"motorsoff",0,simx_opmode_blocking);
	    simxSetObjectPosition(clientID, targetHandler, -1, position, simx_opmode_oneshot);

	    while(1){
			simxGetObjectPosition(clientID, quadricopterHandler, -1, position, simx_opmode_blocking);
			if(position[2] > 0.9){
				break;
			}
	    }
	}

	/*// --> Altura objetivo
	void targetAltitude(double altitude){

		simxFloat * position = new simxFloat[3];
		simxGetObjectPosition(clientID, targetHandler, -1, position, simx_opmode_blocking);

	    //Modificar posicion
	    position[2] = altitude;

	    simxSetObjectPosition(clientID, targetHandler, -1, position, simx_opmode_oneshot);
	}*/

	/************Estado del drone*************/

	// --> Batería 
	int bateryLevel(){
		return 100;
	}

	// --> Intensidad de la conexión
	//todo

	/************Cámara*************/

	// --> Obtener captura de imagen (ambas cámaras)
	cv::Mat* getFrame(Camera cam){

		simxInt cameraHandler;
		if (cam == Camera::Front){
			cameraHandler = quadricopterFrontCamHandler;
		} else {
			cameraHandler = quadricopterFloorCamHandler;
		}

		simxInt resolution[2];
		simxUChar* image;

		image = new simxUChar[512*512*3];
		simxInt aux = simxGetVisionSensorImage(clientID, cameraHandler,resolution,&image,0,simx_opmode_blocking);


		//convertir imagen
		cv::Mat* res = new cv::Mat(resolution[1],resolution[0],CV_8UC3,image);

        cv::cvtColor(*res,*res,cv::COLOR_BGR2RGB);
        cv::flip(*res,*res,0);

		return res;
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

