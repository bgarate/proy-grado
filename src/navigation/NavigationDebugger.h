//
// Created by bruno on 09/08/17.
//

#ifndef PROY_GRADO_NAVIGATIONDEBUGGER_H
#define PROY_GRADO_NAVIGATIONDEBUGGER_H


#include <src/config/Config.h>
#include <pangolin/pangolin.h>
#include "cairo.h"
#include "cairo-xlib.h"
#include "World.h"
#include "MarkerFollower.h"
#include "Path.h"

enum class Axis {
    X,
    Y,
    Z
};

class NavigationDebugger {
public:
    NavigationDebugger(Config* config, World* world);
    void Init();
    void Shutdown();
    void Run(NavigationCommand command, int targetId, std::vector<cv::Vec3d> estimatedPositions,
                 std::vector<cv::Vec3d> estimatedPoses, Path path,
                 boost::circular_buffer<cv::Vec3d, std::allocator<cv::Vec3d>> positionHistor, cv::Vec3d nextPosition,
                 cv::Vec3d predictedNextPosition, cv::Vec3d followTarget);

    void setVisibleMarkers(std::vector<Marker> visibleMarkers);

private:
    Config* config;
    World *world;
    WorldObject* drone;
    Display *dsp;

    std::vector<Marker> visibleMarkers;

    cairo_surface_t* surface;
    cairo_t* cr;

    cairo_surface_t *cairo_create_x11_surface0(int x, int y);

    const int SCALE = 75;
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

    void DrawDrone(NavigationCommand command);

    void DrawMarkerSquare(WorldObject *marker);

    void DrawTargetMarker(PathPoint point);

    void DrawTargetOrientation(WorldObject *marker);

    void DrawMarkerTextId(WorldObject *marker);

    void DrawDroneEstimatedPositions(const std::vector<cv::Vec3d> &estimatedPositions,
                                     const std::vector<cv::Vec3d> &estimatedPoses);

    void DrawCoordinates(Axis axis);

    int Get(double x, Axis axis);

    void DrawPath(Path path, int targetId);

    void DrawPositionHistory(const boost::circular_buffer<cv::Vec3d, std::allocator<cv::Vec3d>> &positionHistory);

    void DrawNextPosition(cv::Vec3d vec, cv::Vec3d projectedNextPosition);

    void DrawCross(cv::Vec3d v, double size);

    void DrawTarget(cv::Vec3d followTarget);
};


#endif //PROY_GRADO_NAVIGATIONDEBUGGER_H
