#include "../hal.hpp"
#include "vrephal.cpp"


int main(int argc, char** argv){

	//Vrephal hal = new Vrephal();
	Vrephal hal;

	while(1){
		hal.hrotate(1, 0.5);
	}
}