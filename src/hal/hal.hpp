#ifndef __HAL_HPP
#define __HAL_HPP

//opencv
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>

//Velocidad de giro por defecto
#define DEFAULT_ROTATE_VEL 1;

//Enumerado cámara
	enum class Camera {Front, Bottom};

	//Definicion frame
	typedef struct {
		char ** data;
		int width, height;
	} Frame;

	//Definicion point
	typedef struct {
		double x;
		double y;
		double z;
	} Point; 

class Hal{
	public:


	/************Movimiento*************/

	//roll = rotate
	//pitch =  backward-forward
	//yaw = derecha-izuquierda
	//gaz = arriba-abajo
	virtual void move(int roll, int pitch, int yaw, int gaz) = 0;

	// --> Rotación horizontal
	virtual void hrotate(double vel) = 0;

	// --> Movimiento horizontal
	virtual void hmove(double angle, double vel) = 0;

	// --> Movimiento vertical
	virtual void vmove(double vel) = 0;

	// --> Despegue y aterrizaje
	virtual void land() = 0;
	virtual void takeoff() = 0;

	// --> Altura objetivo
	virtual void targetAltitude(double altitude) = 0;

	/************Estado del drone*************/

	// --> Batería 
	virtual int bateryLevel() = 0;

	// --> Intensidad de la conexión
	//todo

	/************Cámara*************/

	// --> Obtener captura de imagen (ambas cámaras)
	virtual cv::Mat* getFrame(Camera cam) = 0;

	/************Posición*************/

	// --> Altura
	virtual double getAltitude() = 0;

	// --> Orientación 
	virtual Point getOrientation() = 0;	

	// --> Coordenadas

	virtual Point getGPSPosition() = 0;

};

#endif
