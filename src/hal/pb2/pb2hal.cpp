#include "../hal.hpp"
#include "VideoDecoder.h"
#include "CommandHandler.h"
#include "../Point.h"

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
}

#include <atomic>
#include <boost/thread/mutex.hpp>
#include <libARController/ARController.h>
#include "../../logging/Logger.h"

using namespace std;

class Pb2hal: public Hal {

	//Constantes
	const int PORT = 44444;
	const char * HOST = "192.168.42.1";

    float bottomTilt, frontTilt, defaultPan;

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

    atomic<int> rmoveactive;

    VideoDecoder videoDecoder;
    CommandHandler commandHandler;

	std::shared_ptr<cv::Mat> cvFrame = NULL;
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
        commandHandler.registerHandler(ARCONTROLLER_DICTIONARY_KEY_ARDRONE3_PILOTINGEVENT_MOVEBYEND,
                                       [this](CommandDictionary* d) {this->MoveByEnd(d);});
        commandHandler.registerHandler(ARCONTROLLER_DICTIONARY_KEY_COMMON_CAMERASETTINGSSTATE_CAMERASETTINGSCHANGED,
                                       [this](CommandDictionary* d) {this->CameraSettingsChanged(d);});
        commandHandler.registerHandler(ARCONTROLLER_DICTIONARY_KEY_ARDRONE3_CAMERASTATE_ORIENTATIONV2,
                                       [this](CommandDictionary* d) {this->CameraStateOrientation(d);});

    }

    void MoveByEnd(CommandDictionary* dictionary){
        this->rmoveactive = false;
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

    void CameraSettingsChanged(CommandDictionary* dictionary)
    {
        //dictionary->getFloat(ARCONTROLLER_DICTIONARY_KEY_COMMON_CAMERASETTINGSSTATE_CAMERASETTINGSCHANGED_FOV);
        //dictionary->getFloat(ARCONTROLLER_DICTIONARY_KEY_COMMON_CAMERASETTINGSSTATE_CAMERASETTINGSCHANGED_PANMAX);
        //dictionary->getFloat(ARCONTROLLER_DICTIONARY_KEY_COMMON_CAMERASETTINGSSTATE_CAMERASETTINGSCHANGED_PANMIN);
        //dictionary->getFloat(ARCONTROLLER_DICTIONARY_KEY_COMMON_CAMERASETTINGSSTATE_CAMERASETTINGSCHANGED_TILTMAX);
        if (this->bottomTilt == -1)
            this->bottomTilt = dictionary->getFloat(ARCONTROLLER_DICTIONARY_KEY_COMMON_CAMERASETTINGSSTATE_CAMERASETTINGSCHANGED_TILTMIN);
    }

    void CameraStateOrientation(CommandDictionary* dictionary){

        if(this->frontTilt == -1)
            this->frontTilt = dictionary->getFloat(ARCONTROLLER_DICTIONARY_KEY_ARDRONE3_CAMERASTATE_ORIENTATIONV2_TILT);

        if(this->defaultPan == -1)
            this->defaultPan = dictionary->getFloat(ARCONTROLLER_DICTIONARY_KEY_ARDRONE3_CAMERASTATE_ORIENTATIONV2_PAN);
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

        if (!p2this->videoDecoder.Decode(frame->data,frame->used))
        {
            Logger::logError("Video decode failed");
            return ARCONTROLLER_ERROR_NO_VIDEO;
        }

        uint32_t width = p2this->videoDecoder.GetFrameWidth();
        uint32_t height = p2this->videoDecoder.GetFrameHeight();

        const uint32_t num_bytes = width*height * 3;

        if (num_bytes == 0)
        {
            Logger::logWarning("No picture size");
        }

        if(p2this->cvFrame == NULL){
            p2this->cvFrame = std::shared_ptr<cv::Mat>(new cv::Mat(height, width, CV_8UC3));
        } else {
            if(width != p2this->cvFrame->cols || height != p2this->cvFrame->rows){
				Logger::logWarning("Image dimensions have changed: " +
                                           std::to_string(width) + "x" + std::to_string(height));
				p2this->cvFrame = std::shared_ptr<cv::Mat>(new cv::Mat(height, width, CV_8UC3));
			}
        }

        ARSAL_Sem_Wait(&(p2this->framesem));

        /*if(p2this->frameAvailable)
            Logger::logWarning("Frame lost");*/

        p2this->frameAvailable = true;

        // New frame is ready
        std::copy(p2this->videoDecoder.GetFrameRGBRawCstPtr(),
                  p2this->videoDecoder.GetFrameRGBRawCstPtr() + num_bytes,
                  p2this->cvFrame->data);

        cv::cvtColor(*p2this->cvFrame,*p2this->cvFrame,cv::COLOR_BGR2RGB);


        ARSAL_Sem_Post(&(p2this->framesem));

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

        rmoveactive = false;

        connected = false;

        state = State::Unknown;

        bottomTilt = -1;
        frontTilt = -1;
        defaultPan = -1;
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

        if (state == State::Flying || state == State::Hovering) {
            deviceController->aRDrone3->sendPilotingMoveBy(deviceController->aRDrone3, dx, dy, dz, dh);
            rmoveactive = true;
        } else {
            Logger::logWarning("Cannot rmove: drone isn't flying or hovering");
        }
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

    // --> Elegir de que camara quiero obtener la imagen
    void setCamera(Camera cam){
        if (cam == Camera::Bottom)
            deviceController->aRDrone3->sendCameraOrientationV2(deviceController->aRDrone3, this->bottomTilt, this->defaultPan);
        else
            deviceController->aRDrone3->sendCameraOrientationV2(deviceController->aRDrone3,this->frontTilt, this->defaultPan);
    }

    // --> Obtener captura de imagen (ambas cámaras)
    std::shared_ptr<cv::Mat> getFrame(Camera cam){

        ARSAL_Sem_Wait(&(framesem));

        //if(!frameAvailable)
        //    Logger::logWarning("No frame available");

        frameAvailable = false;

        ARSAL_Sem_Post(&(framesem));
        return cvFrame;
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

    bool isRmoving(){
        return rmoveactive;
    }

};

