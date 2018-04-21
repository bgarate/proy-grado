//
// Created by bruno on 09/08/17.
//


#include "../logging/Logger.h"
#include "../config/ConfigKeys.h"
#include "MapDebugger.h"
#include "../navigation/World.h"
#include "../navigation/MarkerTracker.h"
#include "src/navigation/PathFollower.h"
#include "../navigation/Path.h"
#include <X11/keysym.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <src/communication/SharedMemory.h>

const cv::Size MapDebugger::SIZE = cv::Size(1024,768);
cv::Point MapDebugger::ORIGIN = cv::Point(300,300);
const double MapDebugger::dashPattern[1] = {4.0};

MapDebugger::MapDebugger(Config *config, World* world) :
        positionHistory(200) {
    this->config = config;
    this->world = world;
    drone = world->getDrones()[0];
    this->SCALE = config->Get(ConfigKeys::Debugging::MapDebuggerScale);
}


void MapDebugger::Init() {

    surface = cairo_create_x11_surface0(SIZE.width,SIZE.height);
    cr = cairo_create(surface);
}

cairo_surface_t *MapDebugger::cairo_create_x11_surface0(int x, int y)
{

    int screen;
    cairo_surface_t *sfc;

    if ((dsp = XOpenDisplay(NULL)) == NULL)
        exit(1);
    screen = DefaultScreen(dsp);
    da = XCreateWindow(dsp, DefaultRootWindow(dsp),
                             0, 0, SIZE.width, SIZE.height, 0, 0, CopyFromParent,CopyFromParent, 0, 0);
    XSelectInput(dsp, da, ButtonPressMask|StructureNotifyMask|KeyPressMask|KeyReleaseMask|KeymapStateMask);
    XMapWindow(dsp, da);

    sfc = cairo_xlib_surface_create(dsp, da,
                                    DefaultVisual(dsp, screen), x, y);
    cairo_xlib_surface_set_size(sfc, x, y);

    return sfc;
}

void MapDebugger::Shutdown() {

    Display *dsp = cairo_xlib_surface_get_display(surface);

    cairo_surface_destroy(surface);
    XCloseDisplay(dsp);

}

void MapDebugger::Text(std::string str, cv::Point2i pos, int size = 12) {

    cairo_select_font_face(cr, "Loma Sans",
                           CAIRO_FONT_SLANT_NORMAL,
                           CAIRO_FONT_WEIGHT_BOLD);

    cairo_set_font_size(cr, size);

    cairo_move_to(cr, pos.x, pos.y);

    cairo_show_text(cr, str.c_str());

}

void MapDebugger::DrawAxis(std::string name, cv::Vec3d axis) {

    cairo_set_source_rgb(cr, axis[0],axis[1],axis[2]);
    cairo_move_to(cr, GetX(0),GetY(0));
    cairo_line_to(cr, GetX(axis[0]),GetY(axis[1]));
    cairo_stroke(cr);
    Text(name, cv::Point2i(GetX(axis[0]),GetY(axis[1])));

}

bool MapDebugger::Run(std::map<int, DroneState*> droneStates, int myid, Path path, double deltaTime) {

    ProcessEvents();
    bool ret = ProcessInput(deltaTime);

    if(!ret)
        return false;

    cairo_set_source_rgb(cr, 1,1,1);
    cairo_paint(cr);

    DrawAxis("x", cv::Vec3d(1,0,0));
    DrawAxis("y", cv::Vec3d(0,1,0));
    DrawCoordinates(Axis2::X);
    DrawCoordinates(Axis2::Y);

    for(WorldObject* pad : world->getPads()) {

        DrawPadSquare(pad);
    }

    for(WorldObject* marker : world->getMarkers()) {

        DrawMarkerSquare(marker);

        DrawTargetOrientation(marker);
    }

    for (std::map<int, DroneState*>::iterator it=droneStates.begin(); it!=droneStates.end(); ++it) {
        DrawDrones(it->second, myid);
    }

    for(WorldObject* marker : world->getMarkers()) {
        DrawMarkerTextId(marker);
    }

    DrawPath(path);

    cairo_surface_flush(surface);
    XFlush(dsp);

    return true;
}

void MapDebugger::DrawPath(Path path)  {

    std::vector<PathPoint> points = path.GetPoints();

    for(int i = 0; i < points.size(); i++) {
        PathPoint point = points[i];

        cairo_set_source_rgb (cr, 1, 0, 0);

        cairo_move_to(cr, GetX(point.position[0]),GetY(point.position[1]));

        cv::Point2d arrowEnd(GetX(point.position[0] + 0.5 * sin(toRadians(point.rotation))),
                           GetY(point.position[1] + 0.5 * cos(toRadians(point.rotation))));

        cairo_line_to(cr, arrowEnd.x,arrowEnd.y);
        cairo_stroke(cr);

        cairo_move_to(cr, arrowEnd.x,arrowEnd.y);
        cairo_rel_line_to(cr, GetScaleX(0.15) * sin(toRadians( - point.rotation + ARROW_HEAD_ANGLE)),
                      GetScaleY(0.15) * cos(toRadians( - point.rotation +  ARROW_HEAD_ANGLE)));
        cairo_stroke(cr);

        cairo_move_to(cr, arrowEnd.x,arrowEnd.y);
        cairo_rel_line_to(cr, GetScaleX(0.15) * sin(toRadians( - point.rotation - ARROW_HEAD_ANGLE)),
                          GetScaleY(0.15) * cos(toRadians( - point.rotation - ARROW_HEAD_ANGLE)));
        cairo_stroke(cr);

        PathPoint nextPoint = points[(i + 1) % points.size()];

        cairo_set_source_rgb (cr, 0, 0, 1);
        cairo_move_to(cr, GetX(point.position[0]), GetY(point.position[1]));
        cairo_line_to(cr, GetX(nextPoint.position[0]), GetY(nextPoint.position[1]));
        cairo_stroke(cr);
    }

}

void MapDebugger::DrawMarkerTextId(WorldObject *marker) {
    cairo_set_source_rgb (cr, 1, 0.5, 0.5);
    Text(std::__cxx11::to_string(marker->getId()),
         cv::Point2i(GetX(marker->getPosition()[0] - 0.15), GetY(marker->getPosition()[1] + 0.15)), 8);
}

void MapDebugger::DrawTargetOrientation(WorldObject *marker) {
    double rotationZ = marker->getRotation()[2];
    cairo_set_source_rgb (cr, 0, 1, 0);
    cairo_move_to(cr, GetX(marker->getPosition()[0]), GetY(marker->getPosition()[1]));
    cairo_rel_line_to(cr, GetScaleX(0.15) * sin(toRadians(rotationZ)),
                      -GetScaleY(0.15) * cos(toRadians(rotationZ)));
    cairo_stroke(cr);
}

void MapDebugger::DrawMarkerSquare(WorldObject *marker) {
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

void MapDebugger::DrawPadSquare(WorldObject *pad) {

    cairo_set_source_rgb (cr, 1, 1, 0.7);

    cairo_rectangle (cr, GetX(pad->getPosition()[0] - 0.30),
                     GetY(pad->getPosition()[1] + 0.30), GetScaleX(0.60), GetScaleY(0.60));
    cairo_fill(cr);
}

void MapDebugger::DrawPositionHistory() {

    int historySize = positionHistory.size();

    if(historySize == 0)
        return;

    cairo_set_source_rgb(cr, 1,0,0);
    cairo_move_to (cr, GetX(positionHistory[historySize - 1][0]), GetY(positionHistory[historySize - 1][1]));

    for(int i = historySize - 1; i >= 0; i--) {
        cairo_line_to (cr, GetX(positionHistory[i][0]) + 1, GetY(positionHistory[i][1]) + 1);
        cairo_stroke(cr);

        cairo_move_to (cr, GetX(positionHistory[i][0]) + 1, GetY(positionHistory[i][1]) + 1);

        float interpolation = i/(float)(historySize - 1);
        cairo_set_source_rgb(cr,interpolation,1-interpolation,0);

    }
}


void MapDebugger::DrawDrones(DroneState *drone, int myid){

    if(drone->drone_id() == myid) {
        DrawPositionHistory();
        cairo_set_source_rgb(cr, 0, 1, 0);
    } else {
        cairo_set_source_rgb (cr, 0.75, 0.75, 0.75);
    }


    cairo_save(cr);

    cairo_translate(cr,GetX(drone->position().x()),GetY(drone->position().y()));
    cairo_rotate(cr,toRadians(drone->rotation().z()));

    cairo_arc (cr, GetScaleX(- 0.10), GetScaleY(- 0.10),
               GetScaleX(0.10), 0, M_PI*2);
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

    cairo_set_source_rgb (cr, 1, 0, 0);
    cairo_move_to(cr, 0 , 0);
    cairo_rel_line_to(cr, 0, -GetScaleY(0.25));
    cairo_stroke(cr);

    cairo_restore(cr);

    std::string state = "";
    if (drone->current_task() == DroneState::CurrentTask::DroneState_CurrentTask_INNACTIVE) { state = "INNACTIVE"; }
    if (drone->current_task() == DroneState::CurrentTask::DroneState_CurrentTask_PATROLING) { state = "PATROLING"; }
    if (drone->current_task() == DroneState::CurrentTask::DroneState_CurrentTask_FOLLOWING) { state = "FOLLOWING"; }
    if (drone->current_task() == DroneState::CurrentTask::DroneState_CurrentTask_ALERT) { state = "ALERT"; }
    if (drone->current_task() == DroneState::CurrentTask::DroneState_CurrentTask_CHARGING) { state = "CHARGING"; }
    if (drone->current_task() == DroneState::CurrentTask::DroneState_CurrentTask_CHARGED) { state = "CHARGED"; }
    if (drone->current_task() == DroneState::CurrentTask::DroneState_CurrentTask_BACKFROMPAD) { state = "BACKFROMPAD"; }
    if (drone->current_task() == DroneState::CurrentTask::DroneState_CurrentTask_GOINGTOPAD) { state = "GOINGTOPAD"; }

    cairo_set_source_rgb (cr, 0.5, 0.5, 0.5);
    Text( "Drone "+std::__cxx11::to_string(drone->drone_id())+" "+std::__cxx11::to_string(drone->battery_level())+"%",
         cv::Point2i(GetX(drone->position().x() - 0.30), GetY(drone->position().y() + 0.35)), 9);
    Text(state,
         cv::Point2i(GetX(drone->position().x() - 0.30), GetY(drone->position().y() + 0.25)), 9);

}

int MapDebugger::GetX(double x) {
    return Get(x, Axis2::X);
}

int MapDebugger::Get(double a, Axis2 axis) {
    switch (axis){
        case Axis2::X:
            return (int)(a * SCALE) + ORIGIN.x;
        case Axis2::Y:
            return  SIZE.height - (int)(a * SCALE + ORIGIN.y);
        default:
            throw new std::runtime_error("Invalid axis");
    }
}

int MapDebugger::GetY(double y) {
    return Get(y, Axis2::Y);
}

int MapDebugger::GetScaleX(double x) {
    return (int)(x * SCALE);
}

double MapDebugger::toDegrees(double rad) {
    return rad / M_PI * 180;
}

double MapDebugger::toRadians(double deg) {
    return deg / 180 * M_PI;
}

int MapDebugger::GetScaleY(double y) {
    return (int)(y * SCALE);
}

void MapDebugger::DrawCoordinates(Axis2 axis) {

    int sizeOfLine;
    int sizeOfAxis;
    cv::Point axisDirection;

    switch(axis) {
        case Axis2::X:
            sizeOfLine = SIZE.height;
            sizeOfAxis = SIZE.width;
            axisDirection = cv::Point(1,0);
            break;
        case Axis2::Y:
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

void MapDebugger::ProcessEvents() {

    XEvent event;

    pressedKeys.clear();

    long eventMask = KeyPressMask;

    while(XCheckWindowEvent(dsp,da,eventMask,&event)) {
        unsigned long key;

        switch (event.type) {
            case KeyPress:
                key = XLookupKeysym(&event.xkey, 0);
                pressedKeys.insert(key);
            default:
                break;
        }
    }
}

bool MapDebugger::isKeyPressed(long k) {
    return pressedKeys.find(k) != pressedKeys.end();
}

bool MapDebugger::ProcessInput(double deltaTime) {

    if(isKeyPressed(XK_KP_Subtract))
        SCALE -= 0.00005f * deltaTime;

    if(isKeyPressed(XK_KP_Add))
        SCALE += 0.00005f * deltaTime;

    if(isKeyPressed(XK_Up))
        ORIGIN.y -= 0.0005f * deltaTime;

    if(isKeyPressed(XK_Down))
        ORIGIN.y += 0.0005f * deltaTime;

    if(isKeyPressed(XK_Right))
        ORIGIN.x -= 0.0005f * deltaTime;

    if(isKeyPressed(XK_Left))
        ORIGIN.x += 0.0005f * deltaTime;

    if(isKeyPressed(XK_1))
        forcedState = BrainInfo::PATROLING;
    else if(isKeyPressed(XK_2))
        forcedState = BrainInfo::FOLLOWING;
    else if(isKeyPressed(XK_3))
        forcedState = BrainInfo::GOINGTOPAD;
    else if(isKeyPressed(XK_4))
        forcedState = BrainInfo::BACKFROMPAD;
    else if(isKeyPressed(XK_0))
        forcedState = BrainInfo::INNACTIVE;

    return !isKeyPressed(XK_Escape);

}

bool MapDebugger::isStateForced() {
    return forcedState != BrainInfo::INNACTIVE;
}

BrainInfo::CurrentTask MapDebugger::getForcedState() {
    return forcedState;
}

void MapDebugger::updatePosition(cv::Vec3d position) {
    positionHistory.push_back(position);
}

