//
// Created by bruno on 09/08/17.
//

#ifndef PROY_GRADO_MAPDEBUGGER_H
#define PROY_GRADO_MAPDEBUGGER_H


#include "../config/Config.h"
#include <pangolin/pangolin.h>
#include <src/proto/dronestate.pb.h>
#include <src/communication/SharedMemory.h>
#include "cairo.h"
#include "cairo-xlib.h"
#include "../navigation/World.h"
#include "src/navigation/PathFollower.h"
#include "../navigation/Path.h"

enum class Axis2 {
    X,
    Y,
    Z
};

class MapDebugger {
public:
    /**
     * Constructor
     *
     * @param config clase con los configuración definidos en el parametro de configuración
     * @param world clase con todos las posiciónes de los marcadores y las plataformas de carga
     */
    MapDebugger(Config* config, World* world);

    /**
     * Ininializa el mapa
     */
    void Init();

    /**
     * Elimina el mapa
     */
    void Shutdown();

    /**
     * Step que actualiza la información del mapa
     *
     * @param droneStates conjunto de estados de todos los drones
     * @param myid id del drone que esta mostrando el mapa
     * @param path camino que actual
     * @param deltaTime tiempo pasado desde la última iteración
     * @return
     */
    bool Run(std::map<int, DroneState*> droneStates, int myid, Path path, double deltaTime);

    /**
     * Actualiza los marcadores que son visibles por el drone
     *
     * @param visibleMarkers conjunto de marcadores visibles
     */
    void setVisibleMarkers(std::vector<Marker> visibleMarkers);

    /**
     * Indica la tarea actual que esta realizando el dron está siendo forzado por un comando de debugging
     *
     * @return si el estado es forzado devuelve true, false en caso contrario.
     */
    bool isStateForced();

    /**
     * Devuelve la tarea actual realizada por el dron, en caso de que isStateForced() sea true
     *
     * @return tarea actual que esta realizando el dron
     */
    BrainInfo::CurrentTask getForcedState();

    /**
     * Actualiza la posición actual del drone en el mapa
     *
     * @param position vectro de tres componentes con la posición del dron
     */
    void updatePosition(cv::Vec3d position);

private:
    Config* config;
    World *world;
    WorldObject* drone;
    Display *dsp;
    float SCALE;

    Drawable da;
    // TODO: Desprolijo ponerlo acá
    BrainInfo::CurrentTask forcedState = BrainInfo::INNACTIVE;

    std::set<long> pressedKeys;

    std::vector<Marker> visibleMarkers;

    cairo_surface_t* surface;
    cairo_t* cr;

    cairo_surface_t *cairo_create_x11_surface0(int x, int y);

    boost::circular_buffer<cv::Vec3d> positionHistory;

    const double ARROW_HEAD_ANGLE = 45;
    static const cv::Size SIZE;
    static cv::Point ORIGIN;

    int GetX(double x);

    int GetY(double y);

    int GetScaleY(double y);

    int GetScaleX(double x);

    double toRadians(double deg);

    double toDegrees(double rad);

    void Text(std::string str, cv::Point2i pos, int size);

    void DrawAxis(std::string name, cv::Vec3d axis);

    static const double dashPattern[1];

    void DrawPadSquare(WorldObject *pad);

    void DrawDrones(DroneState *drone, int myid);

    void DrawMarkerSquare(WorldObject *marker);

    void DrawTargetOrientation(WorldObject *marker);

    void DrawMarkerTextId(WorldObject *marker);

    void DrawCoordinates(Axis2 axis);

    int Get(double x, Axis2 axis);

    void DrawPath(Path path);

    void ProcessEvents();

    bool isKeyPressed(long k);

    bool ProcessInput(double deltaTime);

    void DrawPositionHistory();
};


#endif //PROY_GRADO_MAPDEBUGGER_H
