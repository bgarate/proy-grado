#ifndef __HAL_H
#define __HAL_H

//Velocidad de giro por defecto
#define rotateVel 1;

class Hal{
	public:

	//Enumerado cámara
	enum class Camera {Front, Bottom};

	//Definicion frame
	typedef struct {
		unsigned int * data;
		//todo
	} Frame;

	//Definicion point
	typedef struct {
		double x;
		double y;
		double z;
	} Point; 

	/************Movimiento*************/ 

	// --> Rotación horizontal
	void hrotate(double angle, double vel) = 0;

	// --> Movimiento horizontal
	void hmove(angle double, double vel) = 0;

	// --> Movimiento vertical
	void vmove(double angle, double vel) = 0;

	// --> Despegue y aterrizaje
	void land() = 0;
	void takeoff() = 0;

	// --> Altura objetivo
	void targetAltitude(double altitude) = 0;

	/************Estado del drone*************/

	// --> Batería 
	int bateryLevel() = 0;

	// --> Intensidad de la conexión
	//todo

	/************Cámara*************/

	// --> Obtener captura de imagen (ambas cámaras)
	Frame getFrame(Camera cam) = 0;

	/************Posición*************/

	// --> Altura
	double getAltitude() = 0;

	// --> Orientación 
	Point getOrientation() = 0;	

	// --> Coordenadas

	Point getGPSPosition() = 0;

}