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

#include <atomic>

using namespace std;

class Pb2hal: public Hal {

	//Constantes
	const int PORT = 44444;
	const char * HOST = "192.168.42.1"; 

	//Variables aux 
	ARDISCOVERY_Device_t *device = NULL;
	ARCONTROLLER_Device_t *deviceController = NULL;

	atomic<int> batteryLevel;
	
	atomic<double> gpslatitude;
	atomic<double> gpslongitude;
	atomic<double> gpsaltitude;
	
	atomic<double> altitude;

	atomic<double> orientationx;
	atomic<double> orientationy;
	atomic<double> orientationz;

	cv::Mat* lastframe;

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
	static void changedState(eARCONTROLLER_DEVICE_STATE newState, eARCONTROLLER_ERROR error, void *customData){
		Pb2hal * p2this = (Pb2hal*) customData;

	    switch (newState)
	    {
	        case ARCONTROLLER_DEVICE_STATE_RUNNING:
	            break;
	        case ARCONTROLLER_DEVICE_STATE_STOPPED:
	        		ARSAL_Sem_Post(&(p2this->statesem));
	        		
	            break;
	        case ARCONTROLLER_DEVICE_STATE_STARTING:
	            	ARSAL_Sem_Post(&(p2this->statesem));
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
	    if (elementDictionary != NULL){

	    	Pb2hal * p2this = (Pb2hal*) customData;

	    //BATTERY STATE changed
	        if (commandKey == ARCONTROLLER_DICTIONARY_KEY_COMMON_COMMONSTATE_BATTERYSTATECHANGED){

	            ARCONTROLLER_DICTIONARY_ARG_t *arg = NULL;
	            ARCONTROLLER_DICTIONARY_ELEMENT_t *element = NULL;
	            HASH_FIND_STR (elementDictionary, ARCONTROLLER_DICTIONARY_SINGLE_KEY, element);

	            if (element != NULL){

	                HASH_FIND_STR (element->arguments, ARCONTROLLER_DICTIONARY_KEY_COMMON_COMMONSTATE_BATTERYSTATECHANGED_PERCENT, arg);
	                if (arg != NULL){
	                    p2this->batteryLevel = (int) arg->value.U8;
	                }
	            }
	        }
	    //GPS POSITION changed
	        if (commandKey == ARCONTROLLER_DICTIONARY_KEY_ARDRONE3_PILOTINGSTATE_POSITIONCHANGED){
		        
		        ARCONTROLLER_DICTIONARY_ARG_t *arg = NULL;
		        ARCONTROLLER_DICTIONARY_ELEMENT_t *element = NULL;
		        HASH_FIND_STR (elementDictionary, ARCONTROLLER_DICTIONARY_SINGLE_KEY, element);
		        
		        if (element != NULL){
		            
		            HASH_FIND_STR (element->arguments, ARCONTROLLER_DICTIONARY_KEY_ARDRONE3_PILOTINGSTATE_POSITIONCHANGED_LATITUDE, arg);
		            if (arg != NULL){

		                p2this->gpslatitude = arg->value.Double;
		            }

		            HASH_FIND_STR (element->arguments, ARCONTROLLER_DICTIONARY_KEY_ARDRONE3_PILOTINGSTATE_POSITIONCHANGED_LONGITUDE, arg);
		            if (arg != NULL){

		                p2this->gpslongitude = arg->value.Double;
		            }

		            HASH_FIND_STR (element->arguments, ARCONTROLLER_DICTIONARY_KEY_ARDRONE3_PILOTINGSTATE_POSITIONCHANGED_ALTITUDE, arg);
		            if (arg != NULL){

		                p2this->gpsaltitude = arg->value.Double;
		            }
		        }
		    }
		//ALTITUDE changed
		    if (commandKey == ARCONTROLLER_DICTIONARY_KEY_ARDRONE3_PILOTINGSTATE_ALTITUDECHANGED){
		        
		        ARCONTROLLER_DICTIONARY_ARG_t *arg = NULL;
		        ARCONTROLLER_DICTIONARY_ELEMENT_t *element = NULL;
		        HASH_FIND_STR (elementDictionary, ARCONTROLLER_DICTIONARY_SINGLE_KEY, element);
		        
		        if (element != NULL){
		            
		            HASH_FIND_STR (element->arguments, ARCONTROLLER_DICTIONARY_KEY_ARDRONE3_PILOTINGSTATE_ALTITUDECHANGED_ALTITUDE, arg);
		            if (arg != NULL){
		                
		                p2this->altitude = arg->value.Double;
		            }
		        }
		    }
		//ORIENTATION changed
		    if (commandKey == ARCONTROLLER_DICTIONARY_KEY_ARDRONE3_PILOTINGEVENT_MOVEBYEND){

		        ARCONTROLLER_DICTIONARY_ARG_t *arg = NULL;
		        ARCONTROLLER_DICTIONARY_ELEMENT_t *element = NULL;
		        HASH_FIND_STR (elementDictionary, ARCONTROLLER_DICTIONARY_SINGLE_KEY, element);
		        
		        if (element != NULL){


		            HASH_FIND_STR (element->arguments, ARCONTROLLER_DICTIONARY_KEY_ARDRONE3_PILOTINGEVENT_MOVEBYEND_DX, arg);
		            if (arg != NULL){
		                
		                p2this->orientationx = arg->value.Float;
		            }

		            HASH_FIND_STR (element->arguments, ARCONTROLLER_DICTIONARY_KEY_ARDRONE3_PILOTINGEVENT_MOVEBYEND_DY, arg);
		            if (arg != NULL){

		                p2this->orientationy = arg->value.Float;
		            }

		            HASH_FIND_STR (element->arguments, ARCONTROLLER_DICTIONARY_KEY_ARDRONE3_PILOTINGEVENT_MOVEBYEND_DZ, arg);
		            if (arg != NULL){

		                p2this->orientationz = arg->value.Float;
		            }


		        }
		    }
	    }
	}


	//VideoStreming auxs
	static eARCONTROLLER_ERROR configDecoderCallback (ARCONTROLLER_Stream_Codec_t codec, void *customData){

		Pb2hal * p2this = (Pb2hal*) customData;

	    // configure your decoder
	    // return ARCONTROLLER_OK if configuration went well
	    // otherwise, return ARCONTROLLER_ERROR. In that case,
	    // configDecoderCallback will be called again
	    return ARCONTROLLER_OK;
	}

	static eARCONTROLLER_ERROR didReceiveFrameCallback (ARCONTROLLER_Frame_t *frame, void *customData){

		// display the frame
	    // return ARCONTROLLER_OK if display went well
	    // otherwise, return ARCONTROLLER_ERROR. In that case,
	    // configDecoderCallback will be called again

		if (frame != NULL){

			Pb2hal * p2this = (Pb2hal*) customData;

			//convertir imagen
			ARSAL_Sem_Wait(&(p2this->framesem));
			
			delete p2this->lastframe;
			p2this->lastframe = new cv::Mat(frame->height,frame->width,CV_8UC3,frame->data);

	        //cv::cvtColor(*lastframe,*lastframe,cv::COLOR_BGR2RGB);
	        //cv::flip(*lastframe,*lastframe,0);

			ARSAL_Sem_Post(&(p2this->framesem));
		} else {

		}

	    return ARCONTROLLER_OK;
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
	ARSAL_Sem_t statesem;
	ARSAL_Sem_t framesem;

	/************Constructor*************/ 

	Pb2hal(){

		ARSAL_Sem_Init(&(statesem),0,0);
		ARSAL_Sem_Init(&(framesem),0,1);

		//Discovery
		device = createDiscoveryDevice(ARDISCOVERY_PRODUCT_BEBOP_2, "bebop2", HOST, PORT);

		//Crear device controller
		eARCONTROLLER_ERROR error = ARCONTROLLER_OK;
		deviceController = ARCONTROLLER_Device_New (device, &error);

		cout << "ARCONTROLLER_Device_New error code: " << ARCONTROLLER_Error_ToString(error) << endl;

		//Funcion que escucha cambios de estados
		error = ARCONTROLLER_Device_AddStateChangedCallback(deviceController, changedState, this);

		cout << "ARCONTROLLER_Device_AddStateChangedCallback error code: " << ARCONTROLLER_Error_ToString(error) << endl;

		//Funcion que recibe comandos del drone
		error = ARCONTROLLER_Device_AddCommandReceivedCallback(deviceController, receivedOnCommand, this);

		cout << "ARCONTROLLER_Device_AddCommandReceivedCallback error code: " << ARCONTROLLER_Error_ToString(error) << endl;

		//Escuchar video Streming
		error = ARCONTROLLER_Device_SetVideoStreamCallbacks(deviceController, configDecoderCallback, didReceiveFrameCallback, NULL , this);
	
		//Start device controller
		error = ARCONTROLLER_Device_Start(deviceController);

		cout << "ARCONTROLLER_Device_Start error code: " << ARCONTROLLER_Error_ToString(error) << endl;

		while (ARCONTROLLER_Device_GetState(deviceController, &error) != ARCONTROLLER_DEVICE_STATE_RUNNING){
			cout << "Esperando estado running" << endl;
		}

		cout << "Estado: " << ARCONTROLLER_Device_GetState(deviceController, &error) << endl;
		cout << "Estado vuelo: " << getFlyingState(deviceController) << endl;

		ARSAL_Sem_Wait(&(statesem));
	}

	void Pb2halBeforeDelete(){

		deleteDeviceController(deviceController);

		eARCONTROLLER_ERROR error = ARCONTROLLER_OK;
		while (ARCONTROLLER_Device_GetState(deviceController, &error) != ARCONTROLLER_DEVICE_STATE_STOPPED){
			cout << "Esperando estado stopped" << endl;
		}

		ARSAL_Sem_Wait(&(statesem));
	}


	/************Movimiento*************/ 

	 void move(int roll, int pitch, int yaw, int gaz){

    	//normalize
        if(roll<-100){roll=0;}else if(roll>100){roll=0;}
        if(pitch<-100){pitch=0;}else if(pitch>100){pitch=0;}
        if(yaw<-100){yaw=-0;}else if(yaw>100){yaw=0;}
        if(gaz<-100){gaz=-0;}else if(gaz>100){gaz=0;}

	 	deviceController->aRDrone3->setPilotingPCMDFlag(deviceController->aRDrone3,0);
		deviceController->aRDrone3->setPilotingPCMDRoll(deviceController->aRDrone3, roll);
		deviceController->aRDrone3->setPilotingPCMDPitch(deviceController->aRDrone3, pitch);
		deviceController->aRDrone3->setPilotingPCMDYaw(deviceController->aRDrone3, yaw);
		deviceController->aRDrone3->setPilotingPCMDGaz(deviceController->aRDrone3, gaz);
		deviceController->aRDrone3->setPilotingPCMDFlag(deviceController->aRDrone3,1);
	 }


	/*// --> Rotación horizontal
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
	}*/

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

	/*// --> Altura objetivo
	void targetgpsaltitude(double gpsaltitude){

		//TODO
	}*/

	/************Estado del drone*************/

	// --> Batería 
	int bateryLevel(){

		return batteryLevel;
	}

	// --> Intensidad de la conexión
	//todo

	/************Cámara*************/

	// --> Obtener captura de imagen (ambas cámaras)
	cv::Mat* getFrame(Camera cam){

		ARSAL_Sem_Init(&(framesem),0,1);
		cv::Mat* aux = new cv::Mat(*lastframe);
		ARSAL_Sem_Init(&(framesem),0,1);
		return aux;
	}

	/************Posición*************/

	// --> Altura
	double getAltitude(){

		return altitude;
	}

	// --> Orientación 
	Point getOrientation(){

	  	Point ori;
	  	ori.x = orientationx;
	  	ori.y = orientationy;
	  	ori.z = orientationz;

		return ori;
	}	

	// --> Coordenadas
	Point getGPSPosition(){

		Point pos;

		pos.x = gpslatitude;
	  	pos.y = gpslongitude;
	  	pos.z = gpsaltitude;

		return pos;
	}


};

