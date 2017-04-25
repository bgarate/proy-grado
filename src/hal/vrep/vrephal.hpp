#include "../hal.hpp"

extern "C" {
     #include "extApi.h"
}
#include <math.h>


class Vrephal: public Hal {

	//Constantes
	const int PORT = 19997;
	const char * HOST = "127.0.0.1"; 

	//Variables aux 
	int clientID;
	simxInt targetHandler, quadricopterHandler, quadricopterFloorCamHandler, quadricopterFrontCamHandler;


	public:

	/************Constructor*************/ 

	Vrephal();


	/************Movimiento*************/ 

	// --> Rotación horizontal
	void hrotate(int dir, double vel);
	// --> Movimiento horizontal
	void hmove(double angle, double vel);
	// --> Movimiento vertical
	void vmove(double vel);

	// --> Despegue y aterrizaje
	void land();
	void takeoff();

	// --> Altura objetivo
	void targetAltitude(double altitude);

	/************Estado del drone*************/

	// --> Batería 
	int bateryLevel();

	// --> Intensidad de la conexión
	//todo

	/************Cámara*************/

	// --> Obtener captura de imagen (ambas cámaras)
	Frame getFrame(Camera cam);

	/************Posición*************/

	// --> Altura
	double getAltitude();
	// --> Orientación 
	Point getOrientation();
	// --> Coordenadas
	Point getGPSPosition();
};

