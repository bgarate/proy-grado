#include "../hal.hpp"
extern "C" {
     #include "extApi.h"
}
#include <math.h>
#include <iostream>
#include <string>
#include <algorithm>
#include <iostream>
//opencv
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>

using namespace std;

class pb2hal: public Hal {

	//Constantes
	const int PORT = 44444;
	const char * HOST = "192.168.42.1"; 

	//Variables aux 
	ARDISCOVERY_Device_t *device = NULL;
	ARCONTROLLER_Device_t *deviceController = NULL;

	/******Funciones auxiliares******/
	//Discovery
	ARDiscovery_Device_t* createDiscoveryDevice(eARDISCOVERY_PRODUCT product, const char *name, const char *ip, int port){

	    eARDISCOVERY_ERROR errorDiscovery = ARDISCOVERY_OK;
	    ARDiscovery_Device_t *device = NULL;

	    if (ip == NULL || port == 0)
	    {
	        fprintf(stderr, "Bad parameters");
	        return device;
	    }
	    if (product < ARDISCOVERY_PRODUCT_NSNETSERVICE || product >= ARDISCOVERY_PRODUCT_BLESERVICE)
	    {
	        fprintf(stderr, "Bad product (not a wifi product)");
	        return device;
	    }

	    device = ARDISCOVERY_Device_New(&errorDiscovery);

	    if (errorDiscovery == ARDISCOVERY_OK)
	    {
	        errorDiscovery = ARDISCOVERY_Device_InitWifi (device, product, name, port);
	    }

	    if (errorDiscovery != ARDISCOVERY_OK)
	    {
	        ARDISCOVERY_Device_Delete(&device);
	    }

	    return device;
	}

	// called when the state of the device controller has changed
	void stateChanged (eARCONTROLLER_DEVICE_STATE newState, eARCONTROLLER_ERROR error, void *customData)
	{
	    switch (newState)
	    {
	        case ARCONTROLLER_DEVICE_STATE_RUNNING:
	            break;
	        case ARCONTROLLER_DEVICE_STATE_STOPPED:
	            break;
	        case ARCONTROLLER_DEVICE_STATE_STARTING:
	            break;
	        case ARCONTROLLER_DEVICE_STATE_STOPPING:
	            break;
	        default:
	            break;
	    }
	}

	error = ARCONTROLLER_Device_AddCommandReceivedCallback(deviceController, onCommandReceived, NULL);

	// called when a command has been received from the drone
	void onCommandReceived (eARCONTROLLER_DICTIONARY_KEY commandKey, ARCONTROLLER_DICTIONARY_ELEMENT_t *elementDictionary, void *customData)
	{
	    if (elementDictionary != NULL)
	    {
	        // if the command received is a battery state changed
	        /*if (commandKey == ARCONTROLLER_DICTIONARY_KEY_COMMON_COMMONSTATE_BATTERYSTATECHANGED)
	        {
	            ARCONTROLLER_DICTIONARY_ARG_t *arg = NULL;
	            ARCONTROLLER_DICTIONARY_ELEMENT_t *element = NULL;

	            // get the command received in the device controller
	            HASH_FIND_STR (elementDictionary, ARCONTROLLER_DICTIONARY_SINGLE_KEY, element);
	            if (element != NULL)
	            {
	                // get the value
	                HASH_FIND_STR (element->arguments, ARCONTROLLER_DICTIONARY_KEY_COMMON_COMMONSTATE_BATTERYSTATECHANGED_PERCENT, arg);

	                if (arg != NULL)
	                {
	                    uint8_t batteryLevel = arg->value.U8;
	                    // do what you want with the battery level
	                }
	            }
	        }*/
	        // else if (commandKey == THE COMMAND YOU ARE INTERESTED IN)
	    }
	}

	//VideoStreming auxs
	/*static eARCONTROLLER_ERROR configDecoderCallback (ARCONTROLLER_Stream_Codec_t codec, void *customData){
	    // configure your decoder
	    // return ARCONTROLLER_OK if configuration went well
	    // otherwise, return ARCONTROLLER_ERROR. In that case,
	    // configDecoderCallback will be called again
	}

	static eARCONTROLLER_ERROR didReceiveFrameCallback (ARCONTROLLER_Frame_t *frame, void *customData){
	    // display the frame
	    // return ARCONTROLLER_OK if display went well
	    // otherwise, return ARCONTROLLER_ERROR. In that case,
	    // configDecoderCallback will be called again
	}*/

	// This function will wait until the device controller is stopped
	void deleteDeviceController(ARCONTROLLER_Device_t *deviceController)
	{
	    if (deviceController == NULL)
	    {
	        return;
	    }

	    eARCONTROLLER_ERROR error = ARCONTROLLER_OK;

	    eARCONTROLLER_DEVICE_STATE state = ARCONTROLLER_Device_GetState(deviceController, &error);
	    if ((error == ARCONTROLLER_OK) && (state != ARCONTROLLER_DEVICE_STATE_STOPPED))
	    {
	        // after that, stateChanged should be called soon
	        error = ARCONTROLLER_Device_Stop (_deviceController);

	        if (error == ARCONTROLLER_OK)
	        {
	            sem_wait(&someSemaphore);
	        }
	        else
	        {
	            fprintf(stderr, "- error:%s", ARCONTROLLER_Error_ToString(error));
	        }
	    }

	    // once the device controller is stopped, we can delete it
	    ARCONTROLLER_Device_Delete(&deviceController);
	}

	//Obtener estado de vuelo
	eARCOMMANDS_ARDRONE3_PILOTINGSTATE_FLYINGSTATECHANGED_STATE getFlyingState(ARCONTROLLER_Device_t *deviceController)
	{
	    eARCOMMANDS_ARDRONE3_PILOTINGSTATE_FLYINGSTATECHANGED_STATE flyingState = ARCOMMANDS_ARDRONE3_PILOTINGSTATE_FLYINGSTATECHANGED_STATE_MAX;
	    eARCONTROLLER_ERROR error;
	    ARCONTROLLER_DICTIONARY_ELEMENT_t *elementDictionary = ARCONTROLLER_ARDrone3_GetCommandElements(deviceController->aRDrone3, ARCONTROLLER_DICTIONARY_KEY_ARDRONE3_PILOTINGSTATE_FLYINGSTATECHANGED, &error);
	    if (error == ARCONTROLLER_OK && elementDictionary != NULL)
	    {
	        ARCONTROLLER_DICTIONARY_ARG_t *arg = NULL;
	        ARCONTROLLER_DICTIONARY_ELEMENT_t *element = NULL;
	        HASH_FIND_STR (elementDictionary, ARCONTROLLER_DICTIONARY_SINGLE_KEY, element);
	        if (element != NULL)
	        {
	            // Get the value
	            HASH_FIND_STR(element->arguments, ARCONTROLLER_DICTIONARY_KEY_ARDRONE3_PILOTINGSTATE_FLYINGSTATECHANGED_STATE, arg);
	            if (arg != NULL)
	            {
	                // Enums are stored as I32
	                flyingState = arg->value.I32;
	            }
	        }
	    }
	    return flyingState
	}

	public:

	/************Constructor*************/ 

	Vrephal(){

		//Discovery
		device = createDiscoveryDevice(ARDISCOVERY_PRODUCT_ARDRONE, "bebop2", HOST, PORT);

		//Crear device controller
		eARCONTROLLER_ERROR error = ARCONTROLLER_OK;
		ARCONTROLLER_Device_t *deviceController = ARCONTROLLER_Device_New (discoveryDevice, &error);

		//Funcion que escucha cambios de estados
		error = ARCONTROLLER_Device_AddStateChangedCallback(deviceController, stateChanged, NULL);

		//Funcion que recibe comandos del drone
		error = ARCONTROLLER_Device_AddCommandReceivedCallback(deviceController, onCommandReceived, NULL);

		//Escuchar video Streming
		//error = ARCONTROLLER_Device_SetVideoStreamCallbacks(_deviceController, configDecoderCallback, didReceiveFrameCallback, NULL , NULL);
	
		//Start device controller
		error = ARCONTROLLER_Device_Start (deviceController);
	}


	/************Movimiento*************/ 


	// --> Rotación horizontal
	void hrotate(double vel){

		//TODO
	}

	// --> Movimiento horizontal
	void hmove(double angle, double vel){

		//TODO
	}

	// --> Movimiento vertical
	void vmove(double vel){

		//TODO
	}

	// --> Despegue y aterrizaje
	void land(){

		if (deviceController == NULL){
	        return;
	    }

	    eARCOMMANDS_ARDRONE3_PILOTINGSTATE_FLYINGSTATECHANGED_STATE flyingState = getFlyingState(deviceController);
	    if (flyingState == ARCOMMANDS_ARDRONE3_PILOTINGSTATE_FLYINGSTATECHANGED_STATE_FLYING 
	    		|| flyingState == ARCOMMANDS_ARDRONE3_PILOTINGSTATE_FLYINGSTATECHANGED_STATE_HOVERING){
	        deviceController->aRDrone3->sendPilotingLanding(deviceController->aRDrone3);
	    }

	    //Esperar que termine
	    sleep(1);
	    while(1){
	    	eARCOMMANDS_ARDRONE3_PILOTINGSTATE_FLYINGSTATECHANGED_STATE flyingState = getFlyingState(deviceController);
	    	if(flyingState == ARCOMMANDS_ARDRONE3_PILOTINGSTATE_FLYINGSTATECHANGED_STATE_LANDED){
	    		break;
	    	}
	    }
	}

	void takeoff(){
		
	    if (deviceController == NULL){
	        return;
	    }

	    if (getFlyingState(deviceController) == ARCOMMANDS_ARDRONE3_PILOTINGSTATE_FLYINGSTATECHANGED_STATE_LANDED){
	        deviceController->aRDrone3->sendPilotingTakeOff(deviceController->aRDrone3);
	    }

	    //Esperar que termine
	    sleep(1);
	    while(1){
	    	eARCOMMANDS_ARDRONE3_PILOTINGSTATE_FLYINGSTATECHANGED_STATE flyingState = getFlyingState(deviceController);
	    	if(flyingState == ARCOMMANDS_ARDRONE3_PILOTINGSTATE_FLYINGSTATECHANGED_STATE_HOVERING 
	    			|| flyingState == ARCOMMANDS_ARDRONE3_PILOTINGSTATE_FLYINGSTATECHANGED_STATE_FLYING){
	    		break;
	    	}
	    }
	}

	// --> Altura objetivo
	void targetAltitude(double altitude){

		//TODO
	}

	/************Estado del drone*************/

	// --> Batería 
	int bateryLevel(){
		//TODO
	}

	// --> Intensidad de la conexión
	//todo

	/************Cámara*************/

	// --> Obtener captura de imagen (ambas cámaras)
	cv::Mat* getFrame(Camera cam){

		//TODO
	}

	/************Posición*************/

	// --> Altura
	double getAltitude(){

		//TODO
	}

	// --> Orientación 
	Point getOrientation(){

		//TODO
	}	

	// --> Coordenadas
	Point getGPSPosition(){

		//TODO
	}


};

