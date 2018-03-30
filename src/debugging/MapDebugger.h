//
// Created by bruno on 09/08/17.
//

#ifndef PROY_GRADO_MAPDEBUGGER_H
#define PROY_GRADO_MAPDEBUGGER_H


#include "../config/Config.h"
#include <pangolin/pangolin.h>
#include <src/proto/dronestate.pb.h>
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
    void Run(std::map<int, DroneState*> droneStates, int myid, Path path);

    void setVisibleMarkers(std::vector<Marker> visibleMarkers);

private:
    Config* config;
    World *world;
    WorldObject* drone;
    Display *dsp;
    int SCALE;

    std::vector<Marker> visibleMarkers;

    cairo_surface_t* surface;
    cairo_t* cr;

    cairo_surface_t *cairo_create_x11_surface0(int x, int y);

    //const int SCALE= 50;
    const double ARROW_HEAD_ANGLE = 45;
    static const cv::Size SIZE;
    static const cv::Point ORIGIN;

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

};


#endif //PROY_GRADO_MAPDEBUGGER_H
