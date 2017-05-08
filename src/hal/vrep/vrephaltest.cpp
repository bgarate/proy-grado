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

	double vel = 1;

	//despegar
	cout << "Despegar..." << endl;
	hal.takeoff();

	sleep(2);

	cv::Mat* res = hal.getFrame(Camera::Front);

	//girar en un sentido
	cout << "Giro positivo..." << endl;
	for(int i = 0; i < 20; i++){
		hal.hrotate(vel);
	}

	sleep(2);

	//girar en el otro
	cout << "Giro negativo..." << endl;
	for(int i = 0; i < 20; i++){
		hal.hrotate(-vel);
	}

	sleep(2);

	//adelante
	cout << "Adelante..." << endl;
	for(int i = 0; i < 20; i++){
		hal.hmove(0, vel);
	}

	sleep(2);

	//atras
	cout << "Atras..." << endl;
	for(int i = 0; i < 20; i++){
		hal.hmove(180, vel);
	}

	sleep(2);

	//cambio latura objetivo
	cout << "Cambio en altura objetivo..." << endl;
	hal.targetAltitude(0.5);

	//a un lado
	cout << "Para un lado..." << endl;
	for(int i = 0; i < 20; i++){
		hal.hmove(90, vel);
	}

	sleep(2);

	//al otro lado
	cout << "Para el otro..." << endl;
	for(int i = 0; i < 20; i++){
		hal.hmove(270, vel);
	}

	sleep(2);

	//arriba
	cout << "Arriba..." << endl;
	for(int i = 0; i < 20; i++){
		hal.vmove(vel);
	}

	sleep(2);

	//abajo
	cout << "Abajo..." << endl;
	for(int i = 0; i < 20; i++){
		hal.vmove(-vel);
	}

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

	//Altura
	cout << "Altura: " << hal.getAltitude() << endl;

	//Orientacion
	Point o = hal.getOrientation();
	cout << "Orientación: [" << o.x << ", " << o.y << ", " << o.z << "]" << endl;

	//Posicion
	Point p = hal.getOrientation();
	cout << "Posición: [" << p.x << ", " << p.y << ", " << p.z << "]" << endl;
}