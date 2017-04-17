#include "../hal.hpp"
extern "C" {
     #include "extApi.h"
}
#include <math.h>
#include <iostream>
#include <string>
#include <algorithm>


class Vrephal: public Hal {

	//Constantes
	const int PORT = 19997;
	const char * HOST = "127.0.0.1"; 

	//Variables aux 
	int clientID;
	simxInt targetHandler, quadricopterHandler, quadricopterFloorCamHandler, quadricopterFrontCamHandler;


	public:

	/************Constructor*************/ 

	Vrephal(){
		//Conectar con V-REP
		clientID = simxStart(HOST, PORT, true, true, 5000, 5); 
		
	    //Obtener handlers
	    simxInt res1 = simxGetObjectHandle(clientID, "Quadricopter_target",&targetHandler,simx_opmode_blocking);
	    simxInt res2 = simxGetObjectHandle(clientID, "Quadricopter",&quadricopterHandler,simx_opmode_blocking);
	    simxInt res3 = simxGetObjectHandle(clientID, "Quadricopter_floorCamera",&quadricopterFloorCamHandler,simx_opmode_blocking);
	    simxInt res4 = simxGetObjectHandle(clientID, "Quadricopter_frontCamera",&quadricopterFrontCamHandler,simx_opmode_blocking);

	    //Enviar señal a motores
	    //simxSetStringSignal(clientID,"deltavel",(const simxUChar*)"1 1 1 1",8,simx_opmode_blocking);
	}


	/************Movimiento*************/ 


	// --> Rotación horizontal
	void hrotate(int dir, double vel){

		/*int auxdir = 1;
		if (dir<0){
			auxdir = -1;
		}

		simxFloat * orientation = new simxFloat[3];
	  	simxGetObjectOrientation(clientID, targetHandler, -1, orientation, simx_opmode_blocking);

	  	//Calcular orientacion
		orientation[2] = orientation[2] + (vel*auxdir*0.1);
		
		simxSetObjectOrientation(clientID, quadricopterHandler, -1, orientation, simx_opmode_blocking);
		simxSetObjectOrientation(clientID, targetHandler, -1, orientation, simx_opmode_blocking);*/

		simxSetIntegerSignal(clientID,"flag",1,simx_opmode_blocking);
		if (dir<0){

			simxSetFloatSignal(clientID,"vel1",-vel,simx_opmode_blocking);
	    	simxSetFloatSignal(clientID,"vel2",vel,simx_opmode_blocking);
	    	simxSetFloatSignal(clientID,"vel3",-vel,simx_opmode_blocking);
	    	simxSetFloatSignal(clientID,"vel4",vel,simx_opmode_blocking);
		} else {

			simxSetFloatSignal(clientID,"vel1",vel,simx_opmode_blocking);
	    	simxSetFloatSignal(clientID,"vel2",-vel,simx_opmode_blocking);
	    	simxSetFloatSignal(clientID,"vel3",vel,simx_opmode_blocking);
	    	simxSetFloatSignal(clientID,"vel4",-vel,simx_opmode_blocking);
		}
    	
    	simxSetIntegerSignal(clientID,"flag",0,simx_opmode_blocking);
		
		//Enviar señal a motores
	    //simxSetStringSignal(clientID,"deltavel",(const simxUChar*)vels.c_str(),7,simx_opmode_blocking);
	}

	// --> Movimiento horizontal
	void hmove(double angle, double vel){

		//calcular vector
		double x = vel*cos(angle*2*M_PI/360.0);
		double y = vel*sin(angle*2*M_PI/360.0);

		simxFloat * position = new simxFloat[3];
		simxGetObjectPosition(clientID, targetHandler, -1, position, simx_opmode_blocking);

	    //Modificar posicion
	    position[0] = position[0] + x*0.1;
	    position[1] = position[1] + y*0.10;

	    simxSetObjectPosition(clientID, targetHandler, -1, position, simx_opmode_oneshot);
	}

	// --> Movimiento vertical
	void vmove(double vel){

		simxFloat * position = new simxFloat[3];
		simxGetObjectPosition(clientID, quadricopterHandler, -1, position, simx_opmode_blocking);

	    //Modificar posicion
	    position[2] = position[2] + vel;

	    simxSetObjectPosition(clientID, targetHandler, -1, position, simx_opmode_oneshot);

	}

	// --> Despegue y aterrizaje
	void land(){

		simxFloat * position = new simxFloat[3];
		simxGetObjectPosition(clientID, quadricopterHandler, -1, position, simx_opmode_blocking);

	    //Modificar posicion
	    position[2] = 0;

	    simxSetObjectPosition(clientID, quadricopterHandler, -1, position, simx_opmode_oneshot);

	}
	void takeoff(){

		simxFloat * position = new simxFloat[3];
		simxGetObjectPosition(clientID, quadricopterHandler, -1, position, simx_opmode_blocking);

	    //Modificar posicion
	    position[2] = 1;

	    simxSetObjectPosition(clientID, targetHandler, -1, position, simx_opmode_oneshot);

	}

	// --> Altura objetivo
	void targetAltitude(double altitude){

		simxFloat * position = new simxFloat[3];
		simxGetObjectPosition(clientID, quadricopterHandler, -1, position, simx_opmode_blocking);

	    //Modificar posicion
	    position[2] = altitude;

	    simxSetObjectPosition(clientID, targetHandler, -1, position, simx_opmode_oneshot);
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
	Frame getFrame(Camera cam){

		simxInt cameraHandler;
		if (cam == Camera::Front){
			cameraHandler = quadricopterFrontCamHandler;
		} else {
			cameraHandler = quadricopterFloorCamHandler;
		}

		simxInt* resolution = new simxInt[2];
		simxUChar** image;

		simxGetVisionSensorImage(clientID, cameraHandler,resolution,image,0,simx_opmode_blocking);

		//convertir imagen
		Frame res;
		res.Width = resolution[0]; 
		res.height = resolution[1];
		res.data = new char*[res.Width];
		for (int i=0; i<res.Width; i++){

			res.data[i]= new char[res.height];
			for(int j=0; j<res.height; j++){
				res.data[i][j]=image[i][j];
			}
		}
		

		return res;
	}

	/************Posición*************/

	// --> Altura
	double getAltitude(){

		simxFloat * position = new simxFloat[3];
		simxGetObjectPosition(clientID, quadricopterHandler, -1, position, simx_opmode_blocking);

		double pos = position[2];
		return pos;
	}

	// --> Orientación 
	Point getOrientation(){

		simxFloat * orientation = new simxFloat[3];
	  	simxGetObjectOrientation(clientID, quadricopterHandler, -1, orientation, simx_opmode_blocking);

	  	Point ori;
	  	ori.x = orientation[0];
	  	ori.y = orientation[1];
	  	ori.z = orientation[2];

		return ori;
	}	

	// --> Coordenadas
	Point getGPSPosition(){

		simxFloat * position = new simxFloat[3];
		simxGetObjectPosition(clientID, quadricopterHandler, -1, position, simx_opmode_blocking);

		Point pos;
	  	pos.x = position[0];
	  	pos.y = position[1];
	  	pos.z = position[2];

		return pos;
	}


};

