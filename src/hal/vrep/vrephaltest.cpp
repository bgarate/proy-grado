#include "../hal.hpp"
#include "vrephal.cpp"
#include <unistd.h>


int main(int argc, char** argv){

	Vrephal hal;
	double vel = 0.5;
	double rotvel = 0.5;


	hal.hrotate(1, rotvel);
	sleep(2);
	hal.hrotate(1, 0);


	hal.hrotate(-1, rotvel);
	sleep(4);
	hal.hrotate(1, 0);

	hal.hrotate(1, rotvel);
	sleep(2);
	hal.hrotate(1, 0);



	//hal.hrotate2(90);

	/*//girar en un sentido
	for(int i = 0; i < 50; i++){
		hal.hrotate(1, vel);
	}

	sleep(2);

	//girar en el otro
	for(int i = 0; i < 50; i++){
		hal.hrotate(-1, vel);
	}

	sleep(2);

	//adelante
	for(int i = 0; i < 20; i++){
		hal.hmove(0, vel);
	}

	sleep(2);

	//atras
	for(int i = 0; i < 20; i++){
		hal.hmove(180, vel);
	}

	sleep(2);

	//a un lado
	for(int i = 0; i < 20; i++){
		hal.hmove(90, vel);
	}

	sleep(2);

	//al otro lado
	for(int i = 0; i < 20; i++){
		hal.hmove(270, vel);
	}

	//al otro lado
	for(int i = 0; i < 20; i++){
		hal.hmove(270, vel);
	}*/		
}