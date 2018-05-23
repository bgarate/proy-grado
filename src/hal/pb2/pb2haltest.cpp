#include "../hal.hpp"
#include "pb2hal.cpp"
#include "Point.h"
#include <unistd.h>
#include <iostream>
#include <stdio.h>
//opencv
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/core/core.hpp>

using namespace std;

int main(int argc, char** argv){

	Pb2hal hal;

	/************** MOVIMIENTO *****************/

	sleep(10);

	//Imagen
	cout << "Probando imagen..." << endl;
	cv::Mat* res = hal.getFrame(Camera::Front);
	cv::namedWindow( "Display window", cv::WINDOW_AUTOSIZE );// Create a window for display.
    cv::imshow( "Display window", *res );                   // Show our image inside it.
    cv::waitKey(0);
	res->release();

	//despegar
	cout << "Despegar..." << endl;
	//hal.takeoff();

	//battery
	cout << "Batería: " << hal.bateryLevel() << endl;
	//Altura
	cout << "Altura: " << hal.getAltitude() << endl;
	//Orientacion
	Point o = hal.getOrientation();
	cout << "Orientación: [" << o.x << ", " << o.y << ", " << o.z << "]" << endl;
	//Posicion
	Point p = hal.getGPSPosition();
	cout << "Posición: [" << p.x << ", " << p.y << ", " << p.z << "]" << endl;


	cout << "Esperar 4s..." << endl;
	//sleep(4);

	/*cout << "Rotando ..." << endl;
	hal.move(0,0,100,0);
	sleep(4);
	hal.move(0,0,0,0);*/

	/*cout << "subir ..." << endl;
	hal.move(0,0,0,100);
	sleep(4);
	hal.move(0,0,0,0);*/

	/*sleep(2);

	cout << "bajar ..." << endl;
	hal.move(0,0,0,-25);
	sleep(1);
	hal.move(0,0,0,0);*/

	/*cout << "atrás ..." << endl;
	hal.move(0,-25,0,0);
	sleep(2);
	hal.move(0,0,0,0);

	sleep(2);

	cout << "derecha ..." << endl;
	hal.move(25,0,0,0);
	sleep(2);
	hal.move(0,0,0,0);

	sleep(2);

	cout << "adelante ..." << endl;
	hal.move(0,25,0,0);
	sleep(2);
	hal.move(0,0,0,0);

	sleep(2);

	cout << "izquierda ..." << endl;
	hal.move(-25,0,0,0);
	sleep(3);
	hal.move(0,0,0,0);*/

	cout << "Esperar 4s..." << endl;
	//sleep(4);

	//aterrizar
	cout << "Aterrizar..." << endl;
	//hal.land();

	hal.Pb2halBeforeDelete();
}