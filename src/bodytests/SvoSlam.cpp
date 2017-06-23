//
// Created by bruno on 18/05/17.
//

#include <zconf.h>
#include <src/logging/Logger.h>
#include <src/tracking/DetectAndTrack.h>
#include <src/tracking/HogDetector.h>
#include <src/tracking/MultiTracker.h>
#include <src/VisualDebugger.h>
#include <lib/ORB_SLAM2/include/System.h>
#include "BodyTest.h"
#include "../hal/hal.hpp"

#include <svo/config.h>
#include <svo/frame_handler_mono.h>
#include <svo/map.h>
#include <svo/frame.h>
#include <vikit/math_utils.h>
#include <vikit/vision.h>
#include <vikit/abstract_camera.h>
#include <vikit/atan_camera.h>
#include <vikit/pinhole_camera.h>




class SvoSlam : public BodyTest {

    Hal* hal;
    int currentStep = 0;
    double currentTime = 0;

    vk::AbstractCamera* cam_;
    svo::FrameHandlerMono* vo_;

    VisualDebugger* visualDebugger;

    long runningTime = 0;

    void InitBodyTest(Hal *hal, Config* config, VisualDebugger* visualDebugger) override {
        this->hal = hal;
        this->visualDebugger = visualDebugger;

        std::shared_ptr<cv::Mat> frame = hal->getFrame(Camera::Front);
        while(frame == NULL){
            frame = hal->getFrame(Camera::Front);
        }

        cam_ = new vk::PinholeCamera(frame->size().width, frame->size().height, 315.5, 315.5, 376.0, 240.0);
        vo_ = new svo::FrameHandlerMono(cam_);
        vo_->start();

        cv::namedWindow("SVO SLAM", cv::WINDOW_AUTOSIZE);
    }

    bool BodyTestStep(double deltaTime) override {

        std::shared_ptr<cv::Mat> frame = hal->getFrame(Camera::Front);
        if(frame != NULL && frame->rows > 0 & frame->cols > 0) {
            cv::Mat grey;
            cv::cvtColor(*frame, grey, CV_BGR2GRAY);
            runningTime += deltaTime;
            vo_->addImage(grey, runningTime);

            if(vo_->lastFrame() != NULL)
            {
                Eigen::Vector3d vec = vo_->lastFrame()->pos();
                printf("(x,y,z): (%f,%f,%f) \n", vec.x() ,vec.y() , vec.z());

                cv::imshow("SVO SLAM", vo_->lastFrame()->img());
                cv::waitKey(1);
            }
        }

        return true;

    }

    void FinishBodyTest() override {
        if(hal->getState() != State::Landed)
            hal->land();
    }

};