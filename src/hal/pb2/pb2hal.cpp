#include "../hal.hpp"

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

//sdk
extern "C"{
	#include <libARSAL/ARSAL.h>
	#include <libARController/ARController.h>
	#include <libARDiscovery/ARDiscovery.h>	
}

using namespace std;



class Pb2hal: public Hal {

	//Constantes
	const int PORT = 44444;
	const char * HOST = "192.168.42.1"; 

	//Variables aux 
	//static ARSAL_Sem_t statesem;
	//ARSAL_Sem_t Pb2hal::statesem;
	//ARSAL_Sem_t statesem;
	ARDISCOVERY_Device_t *device = NULL;
	ARCONTROLLER_Device_t *deviceController = NULL;

	/******Funciones auxiliares******/
	//Discovery
	ARDISCOVERY_Device_t* createDiscoveryDevice(eARDISCOVERY_PRODUCT product, const char *name, const char *ip, int port){

	    eARDISCOVERY_ERROR errorDiscovery = ARDISCOVERY_OK;
	    ARDISCOVERY_Device_t *device = NULL;

	    if (ip == NULL || port == 0)
	    {
	        fprintf(stderr, "Bad parameters");
	        return device;
	    }

	    device = ARDISCOVERY_Device_New(&errorDiscovery);

	    if (errorDiscovery == ARDISCOVERY_OK){
	        
	        errorDiscovery = ARDISCOVERY_Device_InitWifi (device, product, name, ip, port);

	    	cout << "ARDISCOVERY_Device_InitWifi error code: " << ARDISCOVERY_Error_ToString(errorDiscovery) << endl; 

	    } else {

	        ARDISCOVERY_Device_Delete(&device);
	    }

	    cout << "ARDISCOVERY_Device_New error code: " << ARDISCOVERY_Error_ToString(errorDiscovery) << endl;

	    return device;
	}

	// called when the state of the device controller has changed
	static void changedState(eARCONTROLLER_DEVICE_STATE newState, eARCONTROLLER_ERROR error, void *customData)
	{
	    switch (newState)
	    {
	        case ARCONTROLLER_DEVICE_STATE_RUNNING:
	            break;
	        case ARCONTROLLER_DEVICE_STATE_STOPPED:
	        		//ARSAL_Sem_Post(&(statesem));
	        		
	            break;
	        case ARCONTROLLER_DEVICE_STATE_STARTING:
	            	//ARSAL_Sem_Post(&(statesem));
	            break;
	        case ARCONTROLLER_DEVICE_STATE_STOPPING:
	            break;
	        default:
	            break;
	    }
	}

	// called when a command has been received from the drone
	static void receivedOnCommand (eARCONTROLLER_DICTIONARY_KEY commandKey, ARCONTROLLER_DICTIONARY_ELEMENT_t *elementDictionary, void *customData)
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
	static eARCONTROLLER_ERROR configDecoderCallback (ARCONTROLLER_Stream_Codec_t codec, void *customData){
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
	}

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
	        error = ARCONTROLLER_Device_Stop (deviceController);

	        if (error == ARCONTROLLER_OK)
	        {
	            //sem_wait(&someSemaphore);
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
	eARCOMMANDS_ARDRONE3_PILOTINGSTATE_FLYINGSTATECHANGED_STATE getFlyingState(ARCONTROLLER_Device_t *deviceController){
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
	                flyingState = (eARCOMMANDS_ARDRONE3_PILOTINGSTATE_FLYINGSTATECHANGED_STATE)arg->value.I32;
	            }
	        }
	    }
	    return flyingState;
	}

	public:

	/************Constructor*************/ 

	Pb2hal(){

		//ARSAL_Sem_Init(&(statesem),0,0);

		//Discovery
		device = createDiscoveryDevice(ARDISCOVERY_PRODUCT_BEBOP_2, "bebop2", HOST, PORT);

		//Crear device controller
		eARCONTROLLER_ERROR error = ARCONTROLLER_OK;
		deviceController = ARCONTROLLER_Device_New (device, &error);

		cout << "ARCONTROLLER_Device_New error code: " << ARCONTROLLER_Error_ToString(error) << endl;

		//Funcion que escucha cambios de estados
		error = ARCONTROLLER_Device_AddStateChangedCallback(deviceController, changedState, NULL);

		cout << "ARCONTROLLER_Device_AddStateChangedCallback error code: " << ARCONTROLLER_Error_ToString(error) << endl;

		//Funcion que recibe comandos del drone
		error = ARCONTROLLER_Device_AddCommandReceivedCallback(deviceController, receivedOnCommand, NULL);

		cout << "ARCONTROLLER_Device_AddCommandReceivedCallback error code: " << ARCONTROLLER_Error_ToString(error) << endl;

		//Escuchar video Streming
		error = ARCONTROLLER_Device_SetVideoStreamCallbacks(deviceController, configDecoderCallback, didReceiveFrameCallback, NULL , NULL);
	
		//Start device controller
		error = ARCONTROLLER_Device_Start(deviceController);

		cout << "ARCONTROLLER_Device_Start error code: " << ARCONTROLLER_Error_ToString(error) << endl;

		while (ARCONTROLLER_Device_GetState(deviceController, &error) != ARCONTROLLER_DEVICE_STATE_RUNNING){
			cout << "Esperando estado running" << endl;
		}

		cout << "Estado: " << ARCONTROLLER_Device_GetState(deviceController, &error) << endl;
		cout << "Estado vuelo: " << getFlyingState(deviceController) << endl;

		//ARSAL_Sem_Wait(&(statesem));
	}

	void Pb2halBeforeDelete(){

		deleteDeviceController(deviceController);

		while (ARCONTROLLER_Device_GetState(deviceController, &error) != ARCONTROLLER_DEVICE_STATE_STOPPED){
			cout << "Esperando estado stopped" << endl;
		}

		//ARSAL_Sem_Wait(&(statesem));
	}


	/************Movimiento*************/ 

	 void move(int roll, int pitch, int yaw, int gaz){

	 	//TODO
	 }


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
			cout << "Device controller es null" << endl;
	        return;
	    }

	    eARCOMMANDS_ARDRONE3_PILOTINGSTATE_FLYINGSTATECHANGED_STATE flyingState = getFlyingState(deviceController);
	    if (flyingState == ARCOMMANDS_ARDRONE3_PILOTINGSTATE_FLYINGSTATECHANGED_STATE_FLYING 
	    		|| flyingState == ARCOMMANDS_ARDRONE3_PILOTINGSTATE_FLYINGSTATECHANGED_STATE_HOVERING){
	    	cout << "Landeando" << endl;
	        deviceController->aRDrone3->sendPilotingLanding(deviceController->aRDrone3);
	    }

	    //Esperar que termine
	    //sleep(1);
	    while(1){
	    	eARCOMMANDS_ARDRONE3_PILOTINGSTATE_FLYINGSTATECHANGED_STATE flyingState = getFlyingState(deviceController);
	    	if(flyingState == ARCOMMANDS_ARDRONE3_PILOTINGSTATE_FLYINGSTATECHANGED_STATE_LANDED){
	    		cout << "Esperando bajar" << endl;
	    		break;
	    	}
	    }
	    cout << "Bajó" << endl;
	}

	void takeoff(){
		
	    if (deviceController == NULL){
	    	cout << "Device controller es null" << endl;
	        return;
	    }

	    if (getFlyingState(deviceController) == ARCOMMANDS_ARDRONE3_PILOTINGSTATE_FLYINGSTATECHANGED_STATE_LANDED){
	    	cout << "Takeingofiando" << endl;
	        deviceController->aRDrone3->sendPilotingTakeOff(deviceController->aRDrone3);
	    }

	    //Esperar que termine
	    //sleep(1);
	    while(1){
	    	cout << "Esperando subir" << endl;
	    	eARCOMMANDS_ARDRONE3_PILOTINGSTATE_FLYINGSTATECHANGED_STATE flyingState = getFlyingState(deviceController);
	    	if(flyingState == ARCOMMANDS_ARDRONE3_PILOTINGSTATE_FLYINGSTATECHANGED_STATE_HOVERING 
	    			|| flyingState == ARCOMMANDS_ARDRONE3_PILOTINGSTATE_FLYINGSTATECHANGED_STATE_FLYING){
	    		break;
	    	}
	    }
	    cout << "Subió" << endl;
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

