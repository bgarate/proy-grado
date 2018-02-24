//
// Created by bruno on 09/08/17.
//


#include <src/logging/Logger.h>
#include <src/config/ConfigKeys.h>
#include "NavigationDebugger.h"
#include "World.h"
#include "MarkerTracker.h"
#include "MarkerFollower.h"
#include "Path.h"

const cv::Size NavigationDebugger::SIZE = cv::Size(1024,768);
const cv::Point NavigationDebugger::ORIGIN = cv::Point(300,300);
const double NavigationDebugger::dashPattern[1] = {4.0};

NavigationDebugger::NavigationDebugger(Config *config, World* world) {
    this->config = config;
    this->world = world;
    drone = world->getDrones()[0];
    this->SCALE = config->Get(ConfigKeys::Debugging::NavigationDebuggerScale);
}


void NavigationDebugger::Init() {

    surface = cairo_create_x11_surface0(SIZE.width,SIZE.height);
    cr = cairo_create(surface);
}

cairo_surface_t *NavigationDebugger::cairo_create_x11_surface0(int x, int y)
{
    Drawable da;
    int screen;
    cairo_surface_t *sfc;

    if ((dsp = XOpenDisplay(NULL)) == NULL)
        exit(1);
    screen = DefaultScreen(dsp);
    da = XCreateSimpleWindow(dsp, DefaultRootWindow(dsp),
                             0, 0, SIZE.width, SIZE.height, 0, 0, 0);
    XSelectInput(dsp, da, ButtonPressMask | KeyPressMask);
    XMapWindow(dsp, da);

    sfc = cairo_xlib_surface_create(dsp, da,
                                    DefaultVisual(dsp, screen), x, y);
    cairo_xlib_surface_set_size(sfc, x, y);

    return sfc;
}

void NavigationDebugger::Shutdown() {

    Display *dsp = cairo_xlib_surface_get_display(surface);

    cairo_surface_destroy(surface);
    XCloseDisplay(dsp);

}

void NavigationDebugger::Text(std::string str, cv::Point2i pos, int size = 12) {

    cairo_select_font_face(cr, "Loma Sans",
                           CAIRO_FONT_SLANT_NORMAL,
                           CAIRO_FONT_WEIGHT_BOLD);

    cairo_set_font_size(cr, size);

    cairo_move_to(cr, pos.x, pos.y);

    cairo_show_text(cr, str.c_str());

}

void NavigationDebugger::DrawAxis(std::string name, cv::Vec3d axis) {

    cairo_set_source_rgb(cr, axis[0],axis[1],axis[2]);
    cairo_move_to(cr, GetX(0),GetY(0));
    cairo_line_to(cr, GetX(axis[0]),GetY(axis[1]));
    cairo_stroke(cr);
    Text(name, cv::Point2i(GetX(axis[0]),GetY(axis[1])));

}

void NavigationDebugger::Run(NavigationCommand command, int targetId, std::vector<cv::Vec3d> estimatedPositions,
                             std::vector<cv::Vec3d> estimatedPoses, Path path,
                             boost::circular_buffer<cv::Vec3d, std::allocator<cv::Vec3d>> positionHistory, cv::Vec3d nextPosition,
                             cv::Vec3d projectedNextPosition, cv::Vec3d followTarget) {

    cairo_set_source_rgb(cr, 1,1,1);
    cairo_paint(cr);

    DrawAxis("x", cv::Vec3d(1,0,0));
    DrawAxis("y", cv::Vec3d(0,1,0));
    DrawCoordinates(Axis::X);
    DrawCoordinates(Axis::Y);

    for(WorldObject* pad : world->getPads()) {

        DrawPadSquare(pad);
    }

    for(WorldObject* marker : world->getMarkers()) {

        DrawMarkerSquare(marker);

        DrawTargetOrientation(marker);
    }

    for(WorldObject* marker : world->getMarkers()) {
        DrawMarkerTextId(marker);
    }

    DrawPositionHistory(positionHistory);
    DrawNextPosition(nextPosition, projectedNextPosition);
    DrawTarget(followTarget);

    DrawDrone(command);

    DrawDroneEstimatedPositions(estimatedPositions, estimatedPoses);

    DrawPath(path, targetId);

    cairo_surface_flush(surface);
    XFlush(dsp);
}

void NavigationDebugger::DrawPositionHistory(
        const boost::circular_buffer<cv::Vec3d, std::allocator<cv::Vec3d>> &positionHistory) {

    int historySize = positionHistory.size();

    if(historySize == 0)
        return;

    cairo_set_source_rgb(cr, 1,0,0);
    cairo_move_to (cr, GetX(positionHistory[historySize - 1][0]), GetY(positionHistory[historySize - 1][1]));

    for(int i = historySize - 1; i > std::max(0, historySize - 300); i--) {
        cairo_line_to (cr, GetX(positionHistory[i][0]) + 1, GetY(positionHistory[i][1]) + 1);
    }

    cairo_stroke(cr);
}

void NavigationDebugger::DrawDroneEstimatedPositions(const std::vector<cv::Vec3d> &estimatedPositions,
                                                     const std::vector<cv::Vec3d> &estimatedPoses) {
    for(int i = 0; i < estimatedPositions.size(); i++) {
        cairo_set_source_rgb(cr, 1, 0, 0);

        cairo_save(cr);

        cv::Vec3d pos = estimatedPositions[i];
        double pose = estimatedPoses[i][2];

        cairo_translate(cr, GetX(pos[0]), GetY(pos[1]));
        cairo_rotate(cr, toRadians(pose));

        cairo_set_source_rgb(cr, 1, 0, 0);

        cairo_arc(cr, GetScaleX(-0.10), GetScaleY(-0.10),
                  GetScaleX(0.10), 0, M_PI * 2);
        cairo_stroke(cr);
        cairo_arc(cr, GetScaleX(-0.10), GetScaleY(+0.10),
                  GetScaleX(0.10), 0, M_PI * 2);
        cairo_stroke(cr);
        cairo_arc(cr, GetScaleX(+0.10), GetScaleY(-0.10),
                  GetScaleX(0.10), 0, M_PI * 2);
        cairo_stroke(cr);
        cairo_arc(cr, GetScaleX(+0.10), GetScaleY(+0.10),
                  GetScaleX(0.10), 0, M_PI * 2);
        cairo_stroke(cr);

        cairo_set_source_rgb(cr, 0, 1, 0);
        cairo_move_to(cr, 0, 0);
        cairo_rel_line_to(cr, 0, -GetScaleY(0.15));

        cairo_stroke(cr);

        cairo_restore(cr);
    }
}

void NavigationDebugger::DrawPath(Path path, int targetId)  {

    std::vector<PathPoint> points = path.GetPoints();

    for(int i = 0; i < points.size(); i++) {
        PathPoint point = points[i];

        if((targetId + 1) % points.size() == i) {
            DrawTargetMarker(point);
        }

        cairo_set_source_rgb (cr, 1, 0, 0);

        cairo_move_to(cr, GetX(point.Postion[0]),GetY(point.Postion[1]));

        cv::Point2d arrowEnd(GetX(point.Postion[0] + 0.5 * sin(toRadians(point.Rotation))),
                           GetY(point.Postion[1] + 0.5 * cos(toRadians(point.Rotation))));

        cairo_line_to(cr, arrowEnd.x,arrowEnd.y);
        cairo_stroke(cr);

        cairo_move_to(cr, arrowEnd.x,arrowEnd.y);
        cairo_rel_line_to(cr, GetScaleX(0.15) * sin(toRadians( - point.Rotation + ARROW_HEAD_ANGLE)),
                      GetScaleY(0.15) * cos(toRadians( - point.Rotation +  ARROW_HEAD_ANGLE)));
        cairo_stroke(cr);

        cairo_move_to(cr, arrowEnd.x,arrowEnd.y);
        cairo_rel_line_to(cr, GetScaleX(0.15) * sin(toRadians( - point.Rotation - ARROW_HEAD_ANGLE)),
                          GetScaleY(0.15) * cos(toRadians( - point.Rotation - ARROW_HEAD_ANGLE)));
        cairo_stroke(cr);

        PathPoint nextPoint = points[(i + 1) % points.size()];

        cairo_set_source_rgb (cr, 0, 0, 1);
        cairo_move_to(cr, GetX(point.Postion[0]), GetY(point.Postion[1]));
        cairo_line_to(cr, GetX(nextPoint.Postion[0]), GetY(nextPoint.Postion[1]));
        cairo_stroke(cr);
    }

}

void NavigationDebugger::DrawMarkerTextId(WorldObject *marker) {
    cairo_set_source_rgb (cr, 1, 0.5, 0.5);
    Text(std::__cxx11::to_string(marker->getId()),
         cv::Point2i(GetX(marker->getPosition()[0] - 0.15), GetY(marker->getPosition()[1] + 0.15)), 8);
}

void NavigationDebugger::DrawTargetOrientation(WorldObject *marker) {
    double rotationZ = marker->getRotation()[2];
    cairo_set_source_rgb (cr, 0, 1, 0);
    cairo_move_to(cr, GetX(marker->getPosition()[0]), GetY(marker->getPosition()[1]));
    cairo_rel_line_to(cr, GetScaleX(0.15) * sin(toRadians(rotationZ)),
                      -GetScaleY(0.15) * cos(toRadians(rotationZ)));
    cairo_stroke(cr);
}

void NavigationDebugger::DrawTargetMarker(PathPoint point) {
    cairo_set_source_rgb (cr, 1, 1, 1);

    cairo_rectangle (cr, GetX(point.Postion[0] - 0.10),
                     GetY(point.Postion[1] + 0.10), GetScaleX(0.20), GetScaleY(0.20));
    cairo_stroke(cr);

    cairo_set_dash (cr, dashPattern, 1, 0);

    cairo_set_source_rgb (cr, 0, 0, 1);
    cairo_arc(cr, GetX(point.Postion[0]),
              GetY(point.Postion[1]),
              GetScaleX(MarkerFollower::TARGET_APROXIMATION_DISTANCE), 0, 2 * M_PI);


    cairo_stroke(cr);

    cairo_arc(cr, GetX(point.Postion[0]),
              GetY(point.Postion[1]),
              GetScaleX(MarkerFollower::TARGET_REACHED_DISTANCE), 0, 2 * M_PI);

    cairo_stroke(cr);

    cairo_set_dash (cr, NULL, 0, 0);
}

void NavigationDebugger::DrawMarkerSquare(WorldObject *marker) {
    bool markerInSight = any_of(visibleMarkers.begin(), visibleMarkers.end(), [marker](const Marker& m)-> bool {
            return m.Id == marker->getId();
        });

    if(markerInSight) {
            cairo_set_source_rgb (cr, 0, 1, 1);
        } else {
            cairo_set_source_rgb (cr, 0, 0, 1);
        }

    cairo_rectangle (cr, GetX(marker->getPosition()[0] - 0.15),
                     GetY(marker->getPosition()[1] + 0.15), GetScaleX(0.30), GetScaleY(0.30));
    cairo_fill(cr);
}

void NavigationDebugger::DrawPadSquare(WorldObject *pad) {

    cairo_set_source_rgb (cr, 0.5, 0.5, 0.5);

    cairo_rectangle (cr, GetX(pad->getPosition()[0] - 0.30),
                     GetY(pad->getPosition()[1] + 0.30), GetScaleX(0.60), GetScaleY(0.60));
    cairo_fill(cr);
}

void NavigationDebugger::DrawDrone(NavigationCommand command) {
    cairo_set_source_rgb (cr, 0, 1, 0);

    cairo_save(cr);

    cairo_translate(cr,GetX(drone->getPosition()[0]),GetY(drone->getPosition()[1]));
    cairo_rotate(cr,toRadians(drone->getRotation()[2]));

    cairo_arc (cr, GetScaleX(- 0.10), GetScaleY(- 0.10),
                     GetScaleX(0.12), 0, M_PI*2);
    cairo_fill(cr);
    cairo_arc (cr, GetScaleX( - 0.10), GetScaleY( + 0.10),
               GetScaleX(0.10), 0, M_PI*2);
    cairo_fill(cr);
    cairo_arc (cr, GetScaleX( + 0.10), GetScaleY( - 0.10),
               GetScaleX(0.10), 0, M_PI*2);
    cairo_fill(cr);
    cairo_arc (cr, GetScaleX( + 0.10), GetScaleY( + 0.10),
               GetScaleX(0.10), 0, M_PI*2);
    cairo_fill(cr);

    cairo_set_source_rgb (cr, 0, 0, 1);
    cairo_move_to(cr, 0 , 0);
    cairo_rel_line_to(cr, 0, -GetScaleY(1));
    cairo_stroke(cr);

    cairo_set_source_rgb (cr, 0.6, 0.5, 0.8);
    cairo_move_to(cr, 0 , 0);
    cairo_rel_line_to(cr, GetScaleX(command.LateralSpeed) * 3, -GetScaleY(command.ForwardSpeed) * 3);

    cairo_restore(cr);

}

int NavigationDebugger::GetX(double x) {
    return Get(x, Axis::X);
}

int NavigationDebugger::Get(double a, Axis axis) {
    switch (axis){
        case Axis::X:
            return (int)(a * SCALE) + ORIGIN.x;
        case Axis::Y:
            return  SIZE.height - (int)(a * SCALE + ORIGIN.y);
        default:
            throw new std::runtime_error("Invalid axis");
    }
}

int NavigationDebugger::GetY(double y) {
    return Get(y, Axis::Y);
}

int NavigationDebugger::GetScaleX(double x) {
    return (int)(x * SCALE);
}

double NavigationDebugger::toDegrees(double rad) {
    return rad / M_PI * 180;
}

double NavigationDebugger::toRadians(double deg) {
    return deg / 180 * M_PI;
}

int NavigationDebugger::GetScaleY(double y) {
    return (int)(y * SCALE);
}

void NavigationDebugger::setVisibleMarkers(std::vector<Marker> visibleMarkers) {
    this->visibleMarkers = visibleMarkers;
}

void NavigationDebugger::DrawCoordinates(Axis axis) {

    int sizeOfLine;
    int sizeOfAxis;
    cv::Point axisDirection;

    switch(axis) {
        case Axis::X:
            sizeOfLine = SIZE.height;
            sizeOfAxis = SIZE.width;
            axisDirection = cv::Point(1,0);
            break;
        case Axis::Y:
            sizeOfLine  = SIZE.width;
            sizeOfAxis = SIZE.height;
            axisDirection = cv::Point(0,1);
            break;
        default:
            throw new std::runtime_error("Invalid axis");
    }

    int start = -Get(0, axis) / SCALE;
    int end = (sizeOfAxis - Get(0, axis)) / SCALE;

    cairo_set_source_rgb(cr, 0,0,0);
    cairo_set_dash (cr, dashPattern, 1, 0);
    cairo_set_line_width(cr, 0.5);

    for(int i = start; i <= end; i++) {
        cairo_move_to(cr, GetX(i) * axisDirection.x, GetY(-i) * axisDirection.y);
        cairo_rel_line_to(cr, axisDirection.y * sizeOfLine , axisDirection.x * sizeOfLine );
        cairo_stroke(cr);
    }

    cairo_set_dash (cr, NULL, 0, 0);
    cairo_set_line_width(cr, 1);

}

void NavigationDebugger::DrawNextPosition(cv::Vec3d nextPosition, cv::Vec3d projectedNextPosition) {

    cairo_set_source_rgb(cr, 1, 0, 0);
    DrawCross(nextPosition,0.15);
    DrawCross(projectedNextPosition,0.05);

    cairo_move_to(cr, GetX(nextPosition[0]), GetY(nextPosition[1]));
    cairo_line_to(cr, GetX(projectedNextPosition[0]), GetY(projectedNextPosition[1]));
    cairo_stroke(cr);

}

void NavigationDebugger::DrawCross(cv::Vec3d v, double size) {

    cairo_move_to(cr, GetX(v[0] - size/2), GetY(v[1] + size/2));
    cairo_rel_line_to(cr, GetScaleX(size), GetScaleY(size));
    cairo_stroke(cr);

    cairo_move_to(cr, GetX(v[0] - size/2), GetY(v[1] - size/2));
    cairo_rel_line_to(cr, GetScaleX(size), -GetScaleY(size));
    cairo_stroke(cr);

}

void NavigationDebugger::DrawTarget(cv::Vec3d followTarget) {

    cairo_set_source_rgb(cr, 1, 0, 0);
    DrawCross(followTarget,0.15);

}
