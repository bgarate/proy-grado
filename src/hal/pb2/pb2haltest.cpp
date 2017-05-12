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


	//despegar
	cout << "Despegar..." << endl;
	hal.takeoff();

	cout << "Esperar 8s..." << endl;
	sleep(4);

	//aterrizar
	cout << "Aterrizar..." << endl;
	hal.land();


	hal.Pb2halBeforeDelete();
}