#include "../hal.hpp"
#include "vrephal.cpp"
#include <unistd.h>
#include <iostream>
#include <stdio.h>
//opencv
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>

using namespace std;

int main(int argc, char** argv){

	Vrephal hal;

	/************** MOVIMIENTO *****************/

	double vel = 100;
	int msleep = 5;

	//despegar
	cout << "Despegar..." << endl;
	hal.takeoff();

	sleep(2);

	cv::Mat* res = hal.getFrame(Camera::Front);

	sleep(2);

	//girar en un sentido
	cout << "Giro positivo..." << endl;
	hal.move(vel, 0, 0, 0);
	sleep(msleep);
	hal.move(0, 0, 0, 0);

	sleep(4);

	//girar en el otro
	cout << "Giro negativo..." << endl;
	hal.move(-vel, 0, 0, 0);
	sleep(msleep);
	hal.move(0, 0, 0, 0);

	sleep(4);

	//adelante
	cout << "Adelante..." << endl;
	hal.move(0, vel, 0, 0);
	sleep(msleep);
	hal.move(0, 0, 0, 0);

	sleep(2);

	//atras
	hal.move(0, -vel, 0, 0);
	sleep(msleep);
	hal.move(0, 0, 0, 0);

	sleep(2);

	//a un lado
	cout << "Para un lado..." << endl;
	hal.move(0, 0, vel, 0);
	sleep(msleep);
	hal.move(0, 0, 0, 0);

	sleep(2);

	//al otro lado
	hal.move(0, 0, -vel, 0);
	sleep(msleep);
	hal.move(0, 0, 0, 0);

	sleep(2);

	//arriba
	hal.move(0, 0, 0, vel);
	sleep(msleep);
	hal.move(0, 0, 0, 0);

	sleep(2);

	//abajo
	hal.move(0, 0, 0, -vel);
	sleep(msleep);
	hal.move(0, 0, 0, 0);

	sleep(2);


	//aterrizar
	cout << "Aterrizar..." << endl;
	hal.land();

	sleep(2);

	/************** INFORMACION *****************/

	//Imagen
	cv::namedWindow( "Display window", cv::WINDOW_AUTOSIZE );// Create a window for display.
    cv::imshow( "Display window", *res );                   // Show our image inside it.
    cv::waitKey(0);

	res->release();

	//Altura
	cout << "Altura: " << hal.getAltitude() << endl;

	//Orientacion
	Point o = hal.getOrientation();
	cout << "Orientación: [" << o.x << ", " << o.y << ", " << o.z << "]" << endl;

	//Posicion
	Point p = hal.getOrientation();
	cout << "Posición: [" << p.x << ", " << p.y << ", " << p.z << "]" << endl;
}