//
// Created by bruno on 09/08/17.
//

#include <src/logging/Logger.h>
#include "NavigationDebugger.h"
#include "World.h"
#include "MarkerTracker.h"
#include "MarkerFollower.h"

const cv::Size NavigationDebugger::SIZE = cv::Size(640,480);
const double NavigationDebugger::dashPattern[1] = {4.0};

NavigationDebugger::NavigationDebugger(Config *config, World* world) {
    this->config = config;
    this->world = world;
    drone = world->getDrones()[0];
}


void NavigationDebugger::Init() {

    surface = cairo_create_x11_surface0(640,480);
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

void NavigationDebugger::Run(NavigationCommand command, int targetId,
                             std::vector<cv::Vec3d> estimatedPositions,
                             std::vector<cv::Vec3d> estimatedPoses) {

    cairo_set_source_rgb(cr, 1,1,1);
    cairo_paint(cr);

    DrawAxis("x", cv::Vec3d(1,0,0));
    DrawAxis("y", cv::Vec3d(0,1,0));

    for(WorldObject* marker : world->getMarkers()) {
        cairo_set_source_rgb (cr, 0, 0, 1);

        cairo_rectangle (cr, GetX(marker->getPosition()[0] - 0.15),
                         GetY(marker->getPosition()[1] + 0.15), GetScaleX(0.30), GetScaleY(0.30));
        cairo_fill(cr);

        if(targetId == marker->getId()) {
            cairo_set_source_rgb (cr, 1, 1, 1);

            cairo_rectangle (cr, GetX(marker->getPosition()[0] - 0.10),
                             GetY(marker->getPosition()[1] + 0.10), GetScaleX(0.20), GetScaleY(0.20));
            cairo_stroke(cr);

            cairo_set_dash (cr, dashPattern, 1, 0);

            cairo_set_source_rgb (cr, 0, 0, 1);
            cairo_arc(cr, GetX(marker->getPosition()[0]),
                             GetY(marker->getPosition()[1]),
                      GetScaleX(MarkerFollower::TARGET_APROXIMATION_DISTANCE), 0,2*M_PI);


            cairo_stroke(cr);

            cairo_arc(cr, GetX(marker->getPosition()[0]),
                      GetY(marker->getPosition()[1]),
                      GetScaleX(MarkerFollower::TARGET_REACHED_DISTANCE), 0,2*M_PI);

            cairo_stroke(cr);

            cairo_set_dash (cr, NULL, 0, 0);

        }

        double rotationZ = marker->getRotation()[2];
        cairo_set_source_rgb (cr, 0, 1, 0);
        cairo_move_to(cr, GetX(marker->getPosition()[0]),GetY(marker->getPosition()[1]));
        cairo_rel_line_to(cr, GetScaleX(0.15) * std::sin(toRadians(rotationZ)),
                          -GetScaleY(0.15) * std::cos(toRadians(rotationZ)));
        cairo_stroke(cr);
    }


    for(WorldObject* marker : world->getMarkers()) {
        cairo_set_source_rgb (cr, 1, 0.5, 0.5);
        Text(std::to_string(marker->getId()),
             cv::Point2i(GetX(marker->getPosition()[0] - 0.15), GetY(marker->getPosition()[1] + 0.15)), 8);
    }

    for(int i = 0; i < estimatedPositions.size(); i++){
        cairo_set_source_rgb (cr, 1, 0, 0);
        cv::Vec3d pos = estimatedPositions[i];
        double pose = estimatedPoses[i][2];

        cairo_rectangle (cr, GetX(pos[0] - 0.15), GetY(pos[1] + 0.15), GetScaleX(0.30), GetScaleY(0.30));
        cairo_fill(cr);

        cairo_set_source_rgb (cr, 0, 1, 0);
        cairo_move_to(cr, GetX(pos[0]), GetY(pos[1]));
        cairo_rel_line_to(cr, GetScaleX(0.15) * std::sin(toRadians(pose)),
                          -GetScaleY(0.15) * std::cos(toRadians(pose)));
        cairo_stroke(cr);

    }

    cairo_set_source_rgb (cr, 0, 1, 0);

    cairo_rectangle (cr, GetX(drone->getPosition()[0] - 0.15), GetY(drone->getPosition()[1] + 0.15),
                     GetScaleX(0.30), GetScaleY(0.30));
    cairo_fill(cr);

    cairo_set_source_rgb (cr, 0, 0, 1);
    cairo_move_to(cr, GetX(drone->getPosition()[0]) , GetY(drone->getPosition()[1]));
    cairo_rel_line_to(cr, GetScaleX(0.15) * std::sin(toRadians(drone->getRotation()[2])),
                      -GetScaleY(0.15) * std::cos(toRadians(drone->getRotation()[2])));
    cairo_stroke(cr);

    cairo_surface_flush(surface);
    XFlush(dsp);
}

int NavigationDebugger::GetX(double x) {
    return (int)(x * SCALE) + 100;
}

int NavigationDebugger::GetY(double y) {
    return  SIZE.height - (int)(y * SCALE + 100);
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

//
//void NavigationDebugger::Run() {
//
//    // Create OpenGL window in single line
//    pangolin::CreateWindowAndBind("Main",640,480);
//
//    // 3D Mouse handler requires depth testing to be enabled
//    glEnable(GL_DEPTH_TEST);
//
//    // Define Camera Render Object (for view / scene browsing)
//    pangolin::OpenGlRenderState s_cam(
//            pangolin::ProjectionMatrix(640,480,420,420,320,240,0.1,1000),
//                pangolin::ModelViewLookAt(0,0,5, 0,0,0, pangolin::AxisY)
//    );
//
//    const int UI_WIDTH = 180;
//
//    // Add named OpenGL viewport to window and provide 3D Handler
//    pangolin::View& d_cam = pangolin::CreateDisplay()
//            .SetBounds(0.0, 1.0, pangolin::Attach::Pix(UI_WIDTH), 1.0, -640.0f/480.0f)
//            .SetHandler(new pangolin::Handler3D(s_cam));
//
//    // Default hooks for exiting (Esc) and fullscreen (tab).
//    while( !pangolin::ShouldQuit() )
//    {
//        // Clear entire screen
//        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//
//        // Activate efficiently by object
//        d_cam.Activate(s_cam);
//
//        glClearColor(1,1,1,0.5);
//
//        // Render some stuff
//        glColor3f(0,0,00);
//        glLineWidth(1);
//        pangolin::glDrawAxis(5);
//
//        std::vector<WorldObject*> drones = world->getDrones();
//
//        for(WorldObject* drone : drones) {
//
//            glLineWidth(2);
//            glColor3f(0.0f,1.0f,0.0f);
//
//            cv::Vec3d position = drone->getPosition();
//            cv::Vec3d rotation = drone->getRotation();
//            cv::Mat cameraMatrix = drone->getCameraMatrix();
//            cameraMatrix = cameraMatrix.t();
//
//            std::cout << cameraMatrix << std::endl;
//            double halfSize = std::sqrt(MarkerTracker::MARKER_SIDE_LENGTH / 2);
//
//            glPushMatrix();
//            glMultMatrixd(&cameraMatrix.at<double>(0));
//            glRotated(M_PI,1,0,0);
//
//            //glTranslated(position[0], position[1], position[2]);
//            //glRotated(rotation[0] / M_PI * 180, 1,0,0);
//            //glRotated(rotation[1] / M_PI * 180, 0,0,1);
//            //glRotated(rotation[2] / M_PI * 180, 0,0,-1);
//
//
//            //glBegin(GL_LINES);
//            /*
//            glVertex3f(pos[0] - halfSize,pos[1] - halfSize,pos[2]);
//            glVertex3f(pos[0] + halfSize,pos[1] - halfSize,pos[2]);
//            glVertex3f(pos[0] + halfSize,pos[1] + halfSize,pos[2]);
//            glVertex3f(pos[0] - halfSize,pos[1] + halfSize,pos[2]);
//*/
//            pangolin::glDrawAxis(1);
//
//            //glEnd();
//            glPopMatrix();
//
//        }
//
//        std::vector<WorldObject*> markers = world->getMarkers();
//
//        for(WorldObject* marker : markers) {
//
//            glLineWidth(2);
//            glColor3f(1.0f,0.0f,0.0f);
//
//            cv::Vec3d pos = marker->getPosition();
//            float halfSize = std::sqrt(MarkerTracker::MARKER_SIDE_LENGTH / 2);
//
//            glBegin(GL_QUADS);
//            glPushMatrix();
//
//            glVertex3f(pos[0] - halfSize,pos[1] - halfSize,pos[2]);
//            glVertex3f(pos[0] + halfSize,pos[1] - halfSize,pos[2]);
//            glVertex3f(pos[0] + halfSize,pos[1] + halfSize,pos[2]);
//            glVertex3f(pos[0] - halfSize,pos[1] + halfSize,pos[2]);
//
//            glPopMatrix();
//            glEnd();
//
//            glPopMatrix();
//        }
//
//        // Swap frames and Process Events
//        pangolin::FinishFrame();
//    }
//}
