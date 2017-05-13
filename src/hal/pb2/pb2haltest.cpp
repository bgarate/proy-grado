#include "../hal.hpp"
#include "pb2hal.cpp"
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

	Pb2hal hal;

	/************** MOVIMIENTO *****************/


	//Imagen
	cout << "Probando imagen..." << endl;
	cv::Mat* res = hal.getFrame(Camera::Front);
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

	//despegar
	cout << "Despegar..." << endl;
	hal.takeoff();

	cout << "Esperar 4s..." << endl;
	sleep(4);

	//aterrizar
	cout << "Aterrizar..." << endl;
	hal.land();


	hal.Pb2halBeforeDelete();
}