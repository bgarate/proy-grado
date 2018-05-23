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
    MapDebugger(Config* config, World* world);
    void Init();
    void Shutdown();
    bool Run(std::map<int, DroneState*> droneStates, int myid, Path path, double deltaTime);
    void setVisibleMarkers(std::vector<Marker> visibleMarkers);

    bool isStateForced();
    BrainInfo::CurrentTask getForcedState();
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
