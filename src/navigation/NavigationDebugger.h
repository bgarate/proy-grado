//
// Created by bruno on 09/08/17.
//

#ifndef PROY_GRADO_NAVIGATIONDEBUGGER_H
#define PROY_GRADO_NAVIGATIONDEBUGGER_H


#include <src/Config.h>
#include <pangolin/pangolin.h>
#include "cairo.h"
#include "cairo-xlib.h"
#include "World.h"
#include "MarkerFollower.h"


class NavigationDebugger {
public:
    NavigationDebugger(Config* config, World* world);
    void Init();
    void Shutdown();
    void Run(NavigationCommand command,
             int targetId,
             std::vector<cv::Vec3d> estimatedPositions,
             std::vector<cv::Vec3d> estimatedPoses);
private:
    Config* config;
    World *world;
    WorldObject* drone;
    Display *dsp;

    cairo_surface_t* surface;
    cairo_t* cr;

    cairo_surface_t *cairo_create_x11_surface0(int x, int y);

    const int SCALE = 60;
    static const cv::Size SIZE;

    int GetX(double x);

    int GetY(double y);

    int GetScaleY(double y);

    int GetScaleX(double x);

    double toRadians(double deg);

    double toDegrees(double rad);

    void Text(std::string str, cv::Point2i pos, int size);

    void DrawAxis(std::string name, cv::Vec3d axis);

    static const double dashPattern[1];

};


#endif //PROY_GRADO_NAVIGATIONDEBUGGER_H
