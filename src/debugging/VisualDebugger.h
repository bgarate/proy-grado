
//
// Created by bruno on 27/05/17.
//

#ifndef PROY_GRADO_VISUALDEBUGGER_H
#define PROY_GRADO_VISUALDEBUGGER_H


#include <opencv2/core/mat.hpp>
#include <memory>
#include "../config/Config.h"
#include "../hal/Point.h"
#include "../tracking/Track.h"
#include <opencv/cv.hpp>
#include "../tracking/OpticalFlow.h"
#include "../navigation/MarkerTracker.h"
#include "../hal/hal.hpp"
#include "../tracking/Follower.h"
#include "src/navigation/PathFollower.h"
#include "../tracking/MultiTracker.h"

class ISystem;

enum class VisualParameterType {Tilt, Exposure, Saturation, WhiteBalance, OpticalFlow};

class VisualParameter {
public:
    VisualParameter(VisualParameterType type, std::string name, float value, float min, float max, float step) :
            Type(type), Name(name), Value(value), Min(min), Max(max), Step(step) {
        Changed = false;
    }

    VisualParameterType Type;
    std::string Name;
    float Value;
    float Min;
    float Max;
    float Step;
    bool Changed;
};

class VisualDebugger {
public:
    /**
     * Inicializa el visor
     *
     * @param config clase con los parámetros de configuración
     */
    void setup(Config* config);

    /**
     * Actualiza el frame mostrado en el visor
     *
     * @param frame puntero al nuevo frame
     */
    void setFrame(std::shared_ptr<cv::Mat> frame);

    /**
     * Actualiza información adicional sobrempresa en la imagen
     *
     * @param state tarea actual
     * @param battery nivel de batería
     * @param altitude altitud a la que está volando el dron
     * @param gps posición GPS del dron
     * @param orientation vector de rotaciónes que indica la orientación del dron
     * @param fps cantidad de cuadros por segundo a los que se actualiza la imagen
     * @param runningTime tiempo pasado desde el inico de la ejecución
     */
    void setStatus(State state, int battery, double altitude, Point gps, Point orientation, int fps,
                   long runningTime);

    /**
     * Actualiza el sub estado sobre impreso sobre la imagen
     *
     * @param subStatus cadena que indica el sub estado
     */
    void setSubStatus(std::string subStatus);

    /**
     * Muestra o actualiza la imagen del visor
     *
     * @param deltaTime tiempo pasado desde la última iteración
     * @return clave de la tecla persionada si existe, 0 de lo contrario
     */
    int show(long deltaTime);

    /**
     * Actualizar
     *
     * @param tracks
     */

    /**
     * Actualiza los recuadros que indican intrusos detectados en la imagen
     *
     * @param tracks conjunto de recuadros a dibujar
     */
    void setTracks(std::vector<Track> tracks);

    /**
     * Actualiza los puntos a dibjuar sobre los maradores de aterrizaje detectados
     *
     * @param squarePoints vector de puntos donde se encuetran los marcadores
     */
    void setSquareTracks(std::vector<cv::Point> squarePoints);

    /**
     * TODO
     *
     * @param str
     */
    void writeConsole(std::string str);

    /**
     * Limpia la información del visor
     */
    void cleanup();

    /**
     * Actualiza el comando ejectuado para seguir al intruso
     *
     * @param command comando ejecutado por el dron
     */
    void setFollowCommand(FollowCommand command);

    /**
     * TODO
     *
     * @param deltaTime
     */
    void drawMouse(double deltaTime);

    /**
     * Captura imagen del visor
     */
    void captureImage();

    //void setOrbSlam(ORB_SLAM2::System *slam);
    /**
     * Dibuja o actualiza informacion de slam
     */
    void drawOrbSlam();

    /**
     * Dibuja o actualiza el horizonte
     *
     * @param y altura del frame a la cual dibujar el horizonte
     */
    void drawHorizon(int y);

    /**
     * Dibuja o actualiza información de optical flow
     *
     * @param points puntos a dibujar
     */
    void OpticalFlow(OpticalFlowPoints *points);

    /**
     * Dibuja o actualiza los marcadores
     *
     * @param markers conjunto de marcadores
     */
    void ShowMarkers(std::vector<Marker> markers);

    /**
     * Actualiza comando de navegación ejectuando actualmente
     *
     * @param command comando ejectuado
     */
    void setNavigationCommand(NavigationCommand command);

    /**
     * Dibujar secciónes rojas detectadas
     *
     * @param rects rectangulos detectados
     * @param contours contornos detectados
     */
    void setRedTracks(std::vector<cv::Rect> rects,std::vector<std::vector<cv::Point>> contours);

    /**
     * Actualiza la configruación de la cámara sobrepuesta en al imagen
     *
     * @param parameters conjunto de configuraciónes de imagen
     */
    void setParameters(std::vector<VisualParameter> parameters);

    /**
     * Devuelve la configuración de parámetros de la cámara
     *
     * @return  conjunto de configuraciónes de imagen
     */
    std::vector<VisualParameter> getParameters();

    /**
     * Actualiza la información de los sitemas que están activados
     *
     * @param systems conjunto de sistemas y su estado
     */
    void setSystemsState(std::vector<ISystem*> systems);

private:
    Config* config;
    cv::Mat frame;
    std::shared_ptr<cv::Mat> originalFrame;

    static const cv::Scalar WHITE_COLOR;
    static const cv::Scalar GREEN_COLOR;
    static const cv::Scalar GREY_COLOR;
    static const cv::Scalar BLACK_COLOR;
    static const cv::Scalar RED_COLOR;
    static const cv::Scalar BLUE_COLOR;
    static const cv::Scalar CYAN_COLOR;

    static const int CONSOLE_QUEUE_SIZE = 5;
    static const int CONSOLE_FONT = cv::FONT_HERSHEY_PLAIN;
    static const int CONSOLE_FONT_SIZE = 1;
    static const int CONSOLE_FONT_THICKNESS = 1;
    static constexpr const double OUTPUT_FPS = 25;

    std::vector<VisualParameter> visualParameters;
    int currentParameter = 0;

    double altitude = 0;

    std::array<cv::Scalar, 9> colors ={{ cv::Scalar(255,0,0),
                         cv::Scalar(0,255,0),
                         cv::Scalar(0,0,255),
                         cv::Scalar(120,120,0),
                         cv::Scalar(0,120,120),
                         cv::Scalar(120,0,120),
                         cv::Scalar(255,0,120),
                         cv::Scalar(0,120,255),
                         cv::Scalar(255,120,0)
                                               }};

    std::string windowName;
    std::string opticalFlowWindowName;

    std::vector<std::string> console;

    static std::string getStateName(State state);

    cv::Scalar getStateColor(State state);
    cv::VideoWriter rawOutput;
    cv::VideoWriter hudOutput;

    void openWriters(cv::Size frameSize);
    bool shouldOpen;
    std::vector<Track> tracks;

    static void onMouse(int evt, int x, int y, int flag, void *thisPtr);
    cv::Point2i mousePosition;

    Follower* follower;

    //ORB_SLAM2::FrameDrawer *frameDrawer = NULL;

    std::string lastState = "";

    std::string subStatus = "";

    void ProcessInput(int key);

    void ShowParameters();

};


#endif //PROY_GRADO_VISUALDEBUGGER_H
