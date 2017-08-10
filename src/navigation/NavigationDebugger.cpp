//
// Created by bruno on 09/08/17.
//

#include "NavigationDebugger.h"
#include "World.h"
#include "MarkerTracker.h"

NavigationDebugger::NavigationDebugger(Config *config, World* world) {
    this->config = config;
    this->world = world;
}

void NavigationDebugger::Run() {

    // Create OpenGL window in single line
    pangolin::CreateWindowAndBind("Main",640,480);

    // 3D Mouse handler requires depth testing to be enabled
    glEnable(GL_DEPTH_TEST);

    // Define Camera Render Object (for view / scene browsing)
    pangolin::OpenGlRenderState s_cam(
            pangolin::ProjectionMatrix(640,480,420,420,320,240,0.1,1000),
            pangolin::ModelViewLookAt(-0,0.5,-3, 0,0,0, pangolin::AxisY)
    );

    const int UI_WIDTH = 180;

    // Add named OpenGL viewport to window and provide 3D Handler
    pangolin::View& d_cam = pangolin::CreateDisplay()
            .SetBounds(0.0, 1.0, pangolin::Attach::Pix(UI_WIDTH), 1.0, -640.0f/480.0f)
            .SetHandler(new pangolin::Handler3D(s_cam));

    // Add named Panel and bind to variables beginning 'ui'
    // A Panel is just a View with a default layout and input handling
    pangolin::CreatePanel("ui")
            .SetBounds(0.0, 1.0, 0.0, pangolin::Attach::Pix(UI_WIDTH));

    // Default hooks for exiting (Esc) and fullscreen (tab).
    while( !pangolin::ShouldQuit() )
    {
        // Clear entire screen
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Activate efficiently by object
        d_cam.Activate(s_cam);

        glClearColor(1,1,1,0.5);

        // Render some stuff
        glColor3f(0,0,00);

        pangolin::glDrawAxis(5);

        std::vector<WorldObject*> drones = world->getDrones();

        for(WorldObject* drone : drones) {

            glLineWidth(2);
            glColor3f(0.0f,1.0f,0.0f);

            cv::Vec3d pos = drone->getPosition();
            float halfSize = std::sqrt(MarkerTracker::MARKER_SIDE_LENGTH / 2);

            glBegin(GL_QUADS);
            glPushMatrix();

            glVertex3f(pos[0] - halfSize,pos[1] - halfSize,pos[2]);
            glVertex3f(pos[0] + halfSize,pos[1] - halfSize,pos[2]);
            glVertex3f(pos[0] + halfSize,pos[1] + halfSize,pos[2]);
            glVertex3f(pos[0] - halfSize,pos[1] + halfSize,pos[2]);

            glPopMatrix();
            glEnd();

            glPopMatrix();
        }

        // Swap frames and Process Events
        pangolin::FinishFrame();
    }
}
