#ifndef __HAL_HPP
#define __HAL_HPP

//opencv
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <src/Config.h>
#include "memory"

//Enumerado cámara
	enum class Camera {Front, Bottom};

    enum class State {Unknown, Landed, TakingOff, Hovering, Flying, Landing, Emergency, EmergencyLanding};

	//Definicion frame
	typedef struct {
		char ** data;
		int width, height;
	} Frame;

	//Definicion point
	typedef struct Point{
		double x;
		double y;
		double z;

		Point (double x,double y,double z) {
            this->x = x;
            this->y = y;
            this->z = z;
        }
        Point(){
            x = 0;
            y = 0;
            z = 0;
        }

        double Roll(){return x;}
        double Pitch(){return y;}
        double Yaw(){return z;}

        double Latitude(){return x;}
        double Longitude(){return y;}
        double Altitude(){return z;}

	} Point; 

class Hal{
	public:

    virtual void setup(Config* config) = 0;

	/************Movimiento*************/

	//roll = rotate
	//pitch =  backward-forward
	//yaw = derecha-izquierda
	//gaz = arriba-abajo
	virtual void move(int roll, int pitch, int yaw, int gaz) = 0;

	//dx desplazamiento en el ejex
	//dy desplazamiento en el ejey
	//dz desplazamiento en el ejez
	//dh algulo de desplazamiento (en grados)
	virtual void rmove(double dx, double dy, double dz, double dh) = 0;


	// --> Despegue y aterrizaje
	virtual void land() = 0;
	virtual void takeoff() = 0;

	// --> Altura objetivo
	//virtual void targetAltitude(double altitude) = 0;

	/************Estado del drone*************/

	// --> Batería 
	virtual int bateryLevel() = 0;

	// --> Intensidad de la conexión
	//todo

	/************Cámara*************/

	// --> Obtener captura de imagen (ambas cámaras)
	virtual std::shared_ptr<cv::Mat> getFrame(Camera cam) = 0;

	/************Posición*************/

	// --> Altura
	virtual double getAltitude() = 0;

	// --> Orientación 
	virtual Point getOrientation() = 0;	

	// --> Coordenadas

	virtual Point getGPSPosition() = 0;

    virtual void Connect() = 0;

    virtual void Disconnect() = 0;

    virtual State getState() = 0;

};

#endif
