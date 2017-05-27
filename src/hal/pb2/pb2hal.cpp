#include "../hal.hpp"
#include "VideoDecoder.h"
#include "CommandHandler.h"

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
#include <boost/thread/mutex.hpp>
#include <src/logging/Logger.h>
#include <libARController/ARCONTROLLER_Frame.h>

using namespace std;

class Pb2hal: public Hal {

	//Constantes
	const int PORT = 44444;
	const char * HOST = "192.168.42.1";

    Config* config;

	//Variables aux 
	ARCONTROLLER_Device_t *deviceController = NULL;

    atomic<bool> connected;

	atomic<int> batteryLevel;
	
	atomic<double> gpslatitude;
	atomic<double> gpslongitude;
	atomic<double> gpsaltitude;
	
	atomic<double> altitude;

	atomic<double> orientationx;
	atomic<double> orientationy;
	atomic<double> orientationz;

    atomic<State> state;

    VideoDecoder videoDecoder;
    CommandHandler commandHandler;

    bool validCachedFrame = false;
	std::shared_ptr<cv::Mat> cachedFrame = NULL;
    uint8_t* receivedFrame = NULL;
    uint32_t receivedFrameSize = 0;
    int frameBufferSize = 0;

    bool frameAvailable = false;

	/******Funciones auxiliares******/
	//Discovery
	ARDISCOVERY_Device_t* createDiscoveryDevice(eARDISCOVERY_PRODUCT product, const char *name, const char *ip, int port){

	    eARDISCOVERY_ERROR errorDiscovery = ARDISCOVERY_OK;
	    ARDISCOVERY_Device_t *device = NULL;

	    if (ip == NULL || port == 0)
	    {
            throw new runtime_error("Bad parameters");
	    }

	    device = ARDISCOVERY_Device_New(&errorDiscovery);

	    if (errorDiscovery == ARDISCOVERY_OK){
	        errorDiscovery = ARDISCOVERY_Device_InitWifi (device, product, name, ip, port);

            if(errorDiscovery != ARDISCOVERY_OK) {
                throw new runtime_error("ARDISCOVERY_Device_InitWifi error code: " +
                                 string(ARDISCOVERY_Error_ToString(errorDiscovery)));
            }
        } else {

            throw new runtime_error("ARDISCOVERY_Device_New error code: " +
                             string(ARDISCOVERY_Error_ToString(errorDiscovery)));

	        ARDISCOVERY_Device_Delete(&device);
        }


	    return device;
	}

    void setup(Config* config){
        this->config = config;
    }

	// called when the state of the device controller has changed
	static void changedState(eARCONTROLLER_DEVICE_STATE newState, eARCONTROLLER_ERROR error, void *customData){

		Pb2hal * p2this = (Pb2hal*) customData;

	    switch (newState)
	    {
	        case ARCONTROLLER_DEVICE_STATE_RUNNING:

				ARSAL_Sem_Post(&(p2this->statesem));

				break;
	        case ARCONTROLLER_DEVICE_STATE_STOPPED:

				ARSAL_Sem_Post(&(p2this->statesem));

				break;
	        case ARCONTROLLER_DEVICE_STATE_STARTING:

				break;
	        case ARCONTROLLER_DEVICE_STATE_STOPPING:

				break;
	        default:
	            break;
	    }
	}

    static State mapFlyingState(eARCOMMANDS_ARDRONE3_PILOTINGSTATE_FLYINGSTATECHANGED_STATE state) {
        switch (state){
            case ARCOMMANDS_ARDRONE3_PILOTINGSTATE_FLYINGSTATECHANGED_STATE_LANDED:
                return State::Landed;
            case ARCOMMANDS_ARDRONE3_PILOTINGSTATE_FLYINGSTATECHANGED_STATE_TAKINGOFF:
                return State::TakingOff;
            case ARCOMMANDS_ARDRONE3_PILOTINGSTATE_FLYINGSTATECHANGED_STATE_HOVERING:
                return State::Hovering;
            case ARCOMMANDS_ARDRONE3_PILOTINGSTATE_FLYINGSTATECHANGED_STATE_FLYING:
                return State::Flying;
            case ARCOMMANDS_ARDRONE3_PILOTINGSTATE_FLYINGSTATECHANGED_STATE_LANDING:
                return State::Landing;
            case ARCOMMANDS_ARDRONE3_PILOTINGSTATE_FLYINGSTATECHANGED_STATE_EMERGENCY:
                return State::Emergency;
            case ARCOMMANDS_ARDRONE3_PILOTINGSTATE_FLYINGSTATECHANGED_STATE_EMERGENCY_LANDING:
                return State::EmergencyLanding;
            case ARCOMMANDS_ARDRONE3_PILOTINGSTATE_FLYINGSTATECHANGED_STATE_MOTOR_RAMPING:
            case ARCOMMANDS_ARDRONE3_PILOTINGSTATE_FLYINGSTATECHANGED_STATE_MAX:
            case ARCOMMANDS_ARDRONE3_PILOTINGSTATE_FLYINGSTATECHANGED_STATE_USERTAKEOFF:
            default:
                throw new std::runtime_error("Unknown flying state");
        }
    }

    void registerHandlers(){
        commandHandler.registerHandler(ARCONTROLLER_DICTIONARY_KEY_COMMON_COMMONSTATE_BATTERYSTATECHANGED,
                        [this](CommandDictionary* d) {this->BatteryStateChanged(d);});
        commandHandler.registerHandler(ARCONTROLLER_DICTIONARY_KEY_ARDRONE3_PILOTINGSTATE_POSITIONCHANGED,
                                       [this](CommandDictionary* d) {this->GpsPositionChanged(d);});
        commandHandler.registerHandler(ARCONTROLLER_DICTIONARY_KEY_ARDRONE3_PILOTINGSTATE_ALTITUDECHANGED,
                                       [this](CommandDictionary* d) {this->AltitudeChanged(d);});
        commandHandler.registerHandler(ARCONTROLLER_DICTIONARY_KEY_ARDRONE3_PILOTINGSTATE_ATTITUDECHANGED,
                                       [this](CommandDictionary* d) {this->AttitudeChanged(d);});
        commandHandler.registerHandler(ARCONTROLLER_DICTIONARY_KEY_ARDRONE3_PILOTINGSTATE_FLYINGSTATECHANGED,
                                       [this](CommandDictionary* d) {this->FlyingStateChanged(d);});


    }

    void BatteryStateChanged(CommandDictionary* dictionary){
        this->batteryLevel = dictionary->getUInt8(ARCONTROLLER_DICTIONARY_KEY_COMMON_COMMONSTATE_BATTERYSTATECHANGED_PERCENT);
    }

    void FlyingStateChanged(CommandDictionary* dictionary){
        int stateValue = dictionary->getInteger(ARCONTROLLER_DICTIONARY_KEY_ARDRONE3_PILOTINGSTATE_FLYINGSTATECHANGED_STATE);
        this->state = mapFlyingState((eARCOMMANDS_ARDRONE3_PILOTINGSTATE_FLYINGSTATECHANGED_STATE)stateValue);
        Logger::logError("State changed: %u") << (int)(State)this->state;
    }

    void GpsPositionChanged(CommandDictionary* dictionary){
        this->gpslatitude =
                dictionary->getDouble(ARCONTROLLER_DICTIONARY_KEY_ARDRONE3_PILOTINGSTATE_POSITIONCHANGED_LATITUDE);
        this->gpslongitude =
                dictionary->getDouble(ARCONTROLLER_DICTIONARY_KEY_ARDRONE3_PILOTINGSTATE_POSITIONCHANGED_LONGITUDE);
        this->gpsaltitude =
                dictionary->getDouble(ARCONTROLLER_DICTIONARY_KEY_ARDRONE3_PILOTINGSTATE_POSITIONCHANGED_ALTITUDE);
    }

    void AttitudeChanged(CommandDictionary* dictionary){
        this->orientationx =
                dictionary->getFloat(ARCONTROLLER_DICTIONARY_KEY_ARDRONE3_PILOTINGSTATE_ATTITUDECHANGED_ROLL);
        this->orientationy =
                dictionary->getFloat(ARCONTROLLER_DICTIONARY_KEY_ARDRONE3_PILOTINGSTATE_ATTITUDECHANGED_PITCH);
        this->orientationz =
                dictionary->getFloat(ARCONTROLLER_DICTIONARY_KEY_ARDRONE3_PILOTINGSTATE_ATTITUDECHANGED_YAW);
    }

    void AltitudeChanged(CommandDictionary* dictionary){
        this->altitude = dictionary->getDouble(ARCONTROLLER_DICTIONARY_KEY_ARDRONE3_PILOTINGSTATE_ALTITUDECHANGED_ALTITUDE);
    }

    // called when a command has been received from the drone
	static void receivedOnCommand (eARCONTROLLER_DICTIONARY_KEY commandKey, ARCONTROLLER_DICTIONARY_ELEMENT_t *elementDictionary, void *customData)
	{
        Pb2hal * p2this = (Pb2hal*) customData;
        p2this->commandHandler.handle(commandKey, elementDictionary);
	}


	//VideoStreming auxs
	static eARCONTROLLER_ERROR configDecoderCallback (ARCONTROLLER_Stream_Codec_t codec, void *customData){

		Pb2hal * p2this = (Pb2hal*) customData;

		if (codec.type == ARCONTROLLER_STREAM_CODEC_TYPE_H264)
		{
			Logger::logInfo("H264 configuration packet received: #SPS: " +
                            std::to_string(codec.parameters.h264parameters.spsSize) +
                            " #PPS: " + std::to_string(codec.parameters.h264parameters.ppsSize) +
                            " (MP4? " + std::to_string(codec.parameters.h264parameters.isMP4Compliant) + ")");

			if (!p2this->videoDecoder.SetH264Params(
					codec.parameters.h264parameters.spsBuffer,
					codec.parameters.h264parameters.spsSize,
					codec.parameters.h264parameters.ppsBuffer,
					codec.parameters.h264parameters.ppsSize))
			{
				return ARCONTROLLER_ERROR;
			}
		}
		else
		{
			Logger::logWarning("Codec type is not H264");
			return ARCONTROLLER_ERROR;
		}

	    return ARCONTROLLER_OK;
	}

	static eARCONTROLLER_ERROR didReceiveFrameCallback (ARCONTROLLER_Frame_t *frame, void *customData){

        Pb2hal * p2this = (Pb2hal*) customData;

        if (!frame)
        {
            Logger::logWarning("Received frame is NULL");
            return ARCONTROLLER_ERROR_NO_VIDEO;
        }

        ARSAL_Sem_Wait(&(p2this->framesem));

        if(p2this->frameBufferSize < frame->used) {
            Logger::logInfo("Frame buffer resized from %u bytes to %u bytes")
                    << p2this->frameBufferSize << frame->used;
            p2this->frameBufferSize = frame->used;
            p2this->receivedFrame = (uint8_t*)realloc(p2this->receivedFrame, frame->used);
        }

        p2this->receivedFrameSize = frame->used;
        std::copy(frame->data, frame->data + frame->used, p2this->receivedFrame);

        p2this->validCachedFrame = false;
        p2this->frameAvailable = true;

        ARSAL_Sem_Post(&(p2this->framesem));

        /*if(p2this->frameAvailable)
         Logger::logWarning("Frame lost");*/

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

        if(error != ARCONTROLLER_OK) {
            Logger::logError("ARCONTROLLER_Device_GetState error code: " +
                             string(ARCONTROLLER_Error_ToString(error)));
        }

	    if ((error == ARCONTROLLER_OK) && (state != ARCONTROLLER_DEVICE_STATE_STOPPED))
	    {
	        // after that, stateChanged should be called soon
	        error = ARCONTROLLER_Device_Stop(deviceController);

	        if (error == ARCONTROLLER_OK){
				ARSAL_Sem_Wait(&(statesem));
	        } else {
                Logger::logError("ARCONTROLLER_Device_Stop error code: " +
                                 string(ARCONTROLLER_Error_ToString(error)));
            }
	    }

	    // once the device controller is stopped, we can delete it
	    ARCONTROLLER_Device_Delete(&deviceController);

        connected = false;
	}

	//Obtener estado de vuelo
	/*eARCOMMANDS_ARDRONE3_PILOTINGSTATE_FLYINGSTATECHANGED_STATE getFlyingState(ARCONTROLLER_Device_t *deviceController){
	    eARCOMMANDS_ARDRONE3_PILOTINGSTATE_FLYINGSTATECHANGED_STATE flyingState = ARCOMMANDS_ARDRONE3_PILOTINGSTATE_FLYINGSTATECHANGED_STATE_MAX;
	    eARCONTROLLER_ERROR error;
	    ARCONTROLLER_DICTIONARY_ELEMENT_t *elementDictionary = ARCONTROLLER_ARDrone3_GetCommandElements(deviceController->aRDrone3, ARCONTROLLER_DICTIONARY_KEY_ARDRONE3_PILOTINGSTATE_FLYINGSTATECHANGED, &error);
	    if (error == ARCONTROLLER_OK && elementDictionary != NULL)
	    {
	        ARCONTROLLER_DICTIONARY_ARG_t *arg = NULL;
	        ARCONTROLLER_DICTIONARY_ELEMENT_t *element = NULL;
	        HASH_FIND_STR (elementDictionary, ARCONTROLLER_DICTIONARY_SINGLE_KEY, element);
	        if (element != NULL){
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
	}*/

	public:
	ARSAL_Sem_t statesem;
	ARSAL_Sem_t framesem;

	/************Constructor*************/ 

	Pb2hal() {

        ARSAL_Print_SetCallback(customPrintCallback);

        ARSAL_Sem_Init(&(statesem), 0, 0);
        ARSAL_Sem_Init(&(framesem), 0, 1);

        batteryLevel = 100;

        gpslatitude = 0;
        gpslongitude = 0;
        gpsaltitude = 0;

        altitude = 0;

        orientationx = 0;
        orientationy = 0;
        orientationz = 0;

        connected = false;

        state = State::Unknown;

    }

    static int customPrintCallback(eARSAL_PRINT_LEVEL level, const char *tag, const char *format, va_list va) {

        static const int LOG_BUFFER_SIZE = 500;
        static thread_local char logBuffer[LOG_BUFFER_SIZE];

        vsnprintf(logBuffer, (LOG_BUFFER_SIZE - 1), format, va);
        logBuffer[LOG_BUFFER_SIZE- 1] = '\0';

        switch(level) {
            case ARSAL_PRINT_FATAL:
                Logger::logCritical(logBuffer);
                break;
            case ARSAL_PRINT_ERROR:
                Logger::logError(logBuffer);
                break;
            case ARSAL_PRINT_WARNING:
                Logger::logWarning(logBuffer);
                break;
            case ARSAL_PRINT_INFO:
                Logger::logInfo(logBuffer);
                break;
            case ARSAL_PRINT_DEBUG:
                Logger::logDebug(logBuffer);
                break;
            case ARSAL_PRINT_VERBOSE:
                Logger::logDebug(logBuffer);
                break;
            case ARSAL_PRINT_MAX:break;
        }

        return 0;
    }

    void Connect() {
        try {
            if (connected)
                throw new std::runtime_error("Ya conectado!");

            //Discovery
            ARDISCOVERY_Device_t *device = createDiscoveryDevice(ARDISCOVERY_PRODUCT_BEBOP_2, "bebop2", HOST, PORT);

            //Crear device controller
            eARCONTROLLER_ERROR error = ARCONTROLLER_OK;

            deviceController = ARCONTROLLER_Device_New(device, &error);

            if (error != ARCONTROLLER_OK) {
                Logger::logError("ARCONTROLLER_Device_New error code: " +
                                 string(ARCONTROLLER_Error_ToString(error)));
            }

            // ARDISCOVERY_Device_Delete(&device);

            //Funcion que escucha cambios de estados
            error = ARCONTROLLER_Device_AddStateChangedCallback(deviceController, changedState, this);

            if (error != ARCONTROLLER_OK) {
                Logger::logError("ARCONTROLLER_Device_AddStateChangedCallback error code: " +
                                 string(ARCONTROLLER_Error_ToString(error)));
            }

            registerHandlers();

            //Funcion que recibe comandos del drone
            error = ARCONTROLLER_Device_AddCommandReceivedCallback(deviceController, receivedOnCommand, this);

            if (error != ARCONTROLLER_OK) {
                Logger::logError("ARCONTROLLER_Device_AddCommandReceivedCallback error code: " +
                                 string(ARCONTROLLER_Error_ToString(error)));
            }

            //Escuchar video Streming
            error = ARCONTROLLER_Device_SetVideoStreamCallbacks(deviceController, configDecoderCallback,
                                                                didReceiveFrameCallback, NULL, this);

            if (error != ARCONTROLLER_OK) {
                Logger::logError("ARCONTROLLER_Device_SetVideoStreamCallbacks error code: " +
                                 string(ARCONTROLLER_Error_ToString(error)));
            }

            //Start device controller
            error = ARCONTROLLER_Device_Start(deviceController);

            if (error != ARCONTROLLER_OK) {
                Logger::logError("ARCONTROLLER_Device_Start error code: " +
                                 string(ARCONTROLLER_Error_ToString(error)));
            }

            state = State::Landed;

            ARSAL_Sem_Wait(&(statesem));

			connected = true;

            deviceController->aRDrone3->sendMediaStreamingVideoEnable(deviceController->aRDrone3, true);

            Logger::logInfo("Pb2Hal iniciado");

        } catch (const std::runtime_error ex) {
			Logger::logError("Error during initialization: " + string(ex.what()));
            Disconnect();
        }
    }

	void Disconnect(){
        Logger::logInfo("Cleaning up");
		deleteDeviceController(deviceController);
        ARSAL_Sem_Destroy(&statesem);
        Logger::logInfo("Pb2Hal finalizado");
	}


	/************Movimiento*************/ 

	 void move(int roll, int pitch, int yaw, int gaz){

        if (state == State::Flying || state == State::Hovering) {

            //normalize
            if (roll < -100) { roll = 0; } else if (roll > 100) { roll = 0; }
            if (pitch < -100) { pitch = 0; } else if (pitch > 100) { pitch = 0; }
            if (yaw < -100) { yaw = -0; } else if (yaw > 100) { yaw = 0; }
            if (gaz < -100) { gaz = -0; } else if (gaz > 100) { gaz = 0; }

            //deviceController->aRDrone3->setPilotingPCMD(deviceController->aRDrone3,)
            deviceController->aRDrone3->setPilotingPCMDFlag(deviceController->aRDrone3, 0);
            deviceController->aRDrone3->setPilotingPCMDRoll(deviceController->aRDrone3, roll);
            deviceController->aRDrone3->setPilotingPCMDPitch(deviceController->aRDrone3, pitch);
            deviceController->aRDrone3->setPilotingPCMDYaw(deviceController->aRDrone3, yaw);
            deviceController->aRDrone3->setPilotingPCMDGaz(deviceController->aRDrone3, gaz);
            deviceController->aRDrone3->setPilotingPCMDFlag(deviceController->aRDrone3, 1);
        } else {
            Logger::logWarning("Cannot move: drone isn't flying or hovering");
        }
	 }

    void rmove(double dx, double dy, double dz, double dh) {
        //todo
    }

	// --> Despegue y aterrizaje
	void land(){

		ThrowOnInternalError("Land failed");

        Logger::logInfo("Landing");

	    if (state == State::Flying || state == State::Hovering || state == State::TakingOff){
	        deviceController->aRDrone3->sendPilotingLanding(deviceController->aRDrone3);
	    } else {
            Logger::logWarning("Cannot land: drone isn't flying, hovering or taking off");
        }

	}

	void takeoff(){

        ThrowOnInternalError("Takeoff failed");

        Logger::logInfo("Taking off");

	    if (state == State::Landed){
	        deviceController->aRDrone3->sendPilotingTakeOff(deviceController->aRDrone3);
	    } else {
            Logger::logWarning("Cannot take off: drone isn't landed");
        }

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
    std::shared_ptr<cv::Mat> getFrame(Camera cam){

        if(validCachedFrame)
            return cachedFrame;

        ARSAL_Sem_Wait(&(framesem));
        bool res = videoDecoder.Decode(receivedFrame, receivedFrameSize);
        ARSAL_Sem_Post(&(framesem));

        if (!res)
        {
            Logger::logError("Video decode failed");
            return NULL;
        }

        uint32_t width = videoDecoder.GetFrameWidth();
        uint32_t height = videoDecoder.GetFrameHeight();

        const uint32_t num_bytes = width*height * 3;

        if (num_bytes == 0)
        {
            Logger::logError("No picture size");
            return NULL;
        }

        if(cachedFrame == NULL){
            cachedFrame = std::shared_ptr<cv::Mat>(new cv::Mat(height, width, CV_8UC3));
        } else if(width != cachedFrame->cols || height != cachedFrame->rows) {
            Logger::logWarning("Image dimensions have changed: %ux%u") << width << height;
            cachedFrame = std::shared_ptr<cv::Mat>(new cv::Mat(height, width, CV_8UC3));
        }

        // New frame is ready
        std::copy(videoDecoder.GetFrameRGBRawCstPtr(),
                  videoDecoder.GetFrameRGBRawCstPtr() + num_bytes,
                  cachedFrame->data);

        cv::cvtColor(*cachedFrame,*cachedFrame,cv::COLOR_BGR2RGB);

        //if(!frameAvailable)
        //    Logger::logWarning("No frame available");

        frameAvailable = false;
        validCachedFrame = true;

        return cachedFrame;
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


    void ThrowOnInternalError(const std::string &message)
    {
        if (!connected || !deviceController)
        {
            throw std::runtime_error(message);
        }
    }

    State getState() {
        return state;
    }

};

