//
// Created by bruno on 27/05/17.
//

#include <boost/format.hpp>
#include <chrono>
#include "logging/Logger.h"
#include "tracking/DetectAndTrack.h"
#include "VisualDebugger.h"
#include "hal/Point.h"
#include "../lib/ORB_SLAM2/include/System.h"

const cv::Scalar VisualDebugger::WHITE_COLOR = cv::Scalar(255,255,255);
const cv::Scalar VisualDebugger::GREEN_COLOR = cv::Scalar(0,205,0);
const cv::Scalar VisualDebugger::GREY_COLOR = cv::Scalar(205,205,205);
const cv::Scalar VisualDebugger::BLACK_COLOR = cv::Scalar(0,0,0);
const cv::Scalar VisualDebugger::RED_COLOR = cv::Scalar(255,0,0);

void VisualDebugger::setup(Config *config) {
    this->config = config;
    windowName = "VisualDebugger";

    if(config->isVisualDebugEnabled()) {
        cv::namedWindow(windowName, cv::WINDOW_AUTOSIZE);
        cv::setMouseCallback(windowName, VisualDebugger::onMouse, this);
    }


    follower = new Follower(config);
    shouldOpen = config->isOutputRawVideoEnabled() || config->isOutputHudVideoEnabled();

}

void VisualDebugger::onMouse(int evt, int x, int y, int flag, void* thisPtr) {
    ((VisualDebugger*)thisPtr)->mousePosition = cv::Point2i(x,y);
}

void VisualDebugger::setFrame(std::shared_ptr<cv::Mat> frame) {
    if(frame != NULL && (config->isOutputRawVideoEnabled() || config->isOutputHudVideoEnabled() ||
                        config->isVisualDebugEnabled())) {
        originalFrame = frame;
        frame->copyTo(this->frame);

        if(shouldOpen && frame->cols > 0 && frame->rows > 0) {
            openWriters(cv::Size(frame->cols, frame->rows));
        }
    }
}

void VisualDebugger::openWriters(cv::Size frameSize){

    shouldOpen = false;

    std::chrono::milliseconds ms = std::chrono::duration_cast<std::chrono::milliseconds>( std::chrono::system_clock::now().time_since_epoch());
    std::string videoPath = config->getOutputPath() + "/" + std::to_string(ms.count()) + "_";
    std::string rawVideoPath = videoPath + "RAW.avi";
    std::string hudVideoPath = videoPath + "HUD.avi";

    int fourccCode = cv::VideoWriter::fourcc('M','J','P','G');

    if(config->isOutputRawVideoEnabled()) {
        rawOutput.open(rawVideoPath, fourccCode, OUTPUT_FPS, frameSize);
        Logger::logInfo("RAW video output on %s") << rawVideoPath;
    }

    if(config->isOutputHudVideoEnabled()) {
        hudOutput.open(hudVideoPath, fourccCode, OUTPUT_FPS, frameSize);
        Logger::logInfo("HUD video output on %s") << hudVideoPath;
    }
}

void VisualDebugger::setTracks(std::vector<Track> tracks) {

    this->tracks = tracks;

    if(!config->isVisualDebugEnabled() && !config->isOutputHudVideoEnabled())
        return;

    // draw the tracked object
    for (unsigned i = 0; i < tracks.size(); i++) {
        Track track = tracks[i];
        cv::Scalar color =  colors[track.getNumber() % (sizeof(colors)/sizeof(cv::Scalar))];
        std::string number = std::to_string(track.getNumber());

        rectangle(frame, track.getRect(), color, 2, 1);

        cv::Rect2d trackRect = track.getRect();
        cv::Size textSize = cv::getTextSize(number, CONSOLE_FONT, 1, 1, NULL);
        cv::Point textOrigin = cv::Point((int)trackRect.x + trackRect.width - textSize.width,
                                         trackRect.y - textSize.height);
        cv::putText(frame, number, textOrigin, CONSOLE_FONT, 1, color, 1, cv::LINE_AA);
    }

}

void VisualDebugger::captureImage() {
    if(!config->isVisualDebugEnabled() && !config->isOutputHudVideoEnabled())
        return;

    if(originalFrame == NULL || originalFrame->cols * originalFrame->rows == 0)
        return;

    std::chrono::milliseconds ms =
            std::chrono::duration_cast<std::chrono::milliseconds>(
                    std::chrono::system_clock::now().time_since_epoch());

    std::string imgPath = config->getOutputPath() + "/" + std::to_string(ms.count()) + "_Image.jpg";

    cv::imwrite(imgPath, *originalFrame);

    Logger::logInfo("Capture saved to %s") << imgPath;

}

cv::Scalar VisualDebugger::getStateColor(State state){
    switch (state){
        case State::Unknown:
            return WHITE_COLOR;
        case State::Landed:
            return cv::Scalar(255,215,0);
        case State::TakingOff:
            return cv::Scalar(99,184,255);
        case State::Hovering:
            return cv::Scalar(34,139,34);
        case State::Flying:
            return GREEN_COLOR;
        case State::Landing:
            return cv::Scalar(255,193,37);
        case State::Emergency:
            return cv::Scalar(255,0,0);
        case State::EmergencyLanding:
            return cv::Scalar(205,55,0);
    }
}

std::string VisualDebugger::getStateName(State state){
    switch (state){
        case State::Unknown:
            return "Unknown";
        case State::Landed:
            return "Landed";
        case State::TakingOff:
            return "Taking off";
        case State::Hovering:
            return "Hovering";
        case State::Flying:
            return "Flying";
        case State::Landing:
            return "Landing";
        case State::Emergency:
            return "Emergency";
        case State::EmergencyLanding:
            return "Emergency landing";
    }
}

void VisualDebugger::setStatus(State state, int battery, double altitude, Point gps, Point orientation,
    int fps, long runningTime){

    if(!config->isVisualDebugEnabled() && !config->isOutputHudVideoEnabled())
        return;

    if(frame.cols == 0 && frame.rows == 0)
        return;

    this->altitude = altitude;

    std::string statusName = getStateName(state);
    cv::Scalar statusColor =  getStateColor(state);

    cv::Size textSize =
            cv::getTextSize(statusName, CONSOLE_FONT, 2, 1, NULL);

    cv::Point textOrigin = cv::Point(10, textSize.height + 10);

    cv::putText(frame, statusName, cv::Point(textOrigin.x + 2, textOrigin.y + 2), CONSOLE_FONT, 2, BLACK_COLOR, 2, cv::LINE_AA);
    cv::putText(frame, statusName, textOrigin, CONSOLE_FONT, 2, statusColor, 2, cv::LINE_AA);

    long runningSeconds = runningTime / 1000000;
    long runningMinutes = runningSeconds / 60;

    std::vector<std::string> colValues = {
            (boost::format("FPS: %3u Running time: %02um:%02us") % fps % runningMinutes % (runningSeconds % 60)).str(),
            (boost::format("Bateria: %3u%%") % battery).str(),
            (boost::format("Altitud: %3.2fm") % altitude).str(),
            (boost::format("Lat: %3.2f Long: %3.2f Alt: %3.2f") % gps.Latitude() %
             gps.Longitude() % gps.Altitude()).str(),
            (boost::format("Roll: %3.2f Pitch: %3.2f Yaw: %3.2f") % orientation.Roll() %
            orientation.Pitch() % orientation.Yaw()).str(),
    };


    cv::Mat overlay;
    frame.copyTo(overlay);

    cv::rectangle(overlay, cv::Rect(0, frame.rows - 50, frame.cols,50), BLACK_COLOR, cv::FILLED);
    cv::addWeighted(overlay, 0.3, frame, 0.7, 0, frame);

    textSize = cv::getTextSize(colValues[0], CONSOLE_FONT, 1, 1, NULL);
    textOrigin = cv::Point((frame.cols - textSize.width)/2, textSize.height + 10);
    cv::putText(frame, colValues[0], cv::Point(textOrigin.x + 1, textOrigin.y + 1),
                CONSOLE_FONT, 1, BLACK_COLOR, 1, cv::LINE_AA);
    cv::putText(frame, colValues[0], textOrigin, CONSOLE_FONT, 1, GREEN_COLOR, 1, cv::LINE_AA);

    textOrigin = cv::Point(10, frame.rows - 10);
    cv::putText(frame, colValues[1], textOrigin, CONSOLE_FONT, 1, GREEN_COLOR, 1, cv::LINE_AA);

    textSize = cv::getTextSize(colValues[2], CONSOLE_FONT, 1, 1, NULL);
    textOrigin = cv::Point((frame.cols - textSize.width)/2, frame.rows - 10);
    cv::putText(frame, colValues[2], textOrigin, CONSOLE_FONT, 1, GREEN_COLOR, 1, cv::LINE_AA);

    textSize = cv::getTextSize(colValues[3], CONSOLE_FONT, 1, 1, NULL);
    textOrigin = cv::Point(frame.cols - textSize.width - 10, frame.rows - 10);
    cv::putText(frame, colValues[3], textOrigin, CONSOLE_FONT, 1, GREEN_COLOR, 1, cv::LINE_AA);

    cv::Size previousTextSize = textSize;
    textSize = cv::getTextSize(colValues[4], CONSOLE_FONT, 1, 1, NULL);
    textOrigin = cv::Point(frame.cols - textSize.width - 10, frame.rows - previousTextSize.height - 20);
    cv::putText(frame, colValues[4], textOrigin, CONSOLE_FONT, 1, GREEN_COLOR, 1, cv::LINE_AA);

}

void VisualDebugger::drawOrbSlam() {
    if(frameDrawer == NULL)
        return;

    std::string state = frameDrawer->DrawFrame(frame);

    if(state != lastState) {
        writeConsole(state);
        lastState = state;
    }
}

void VisualDebugger::setOrbSlam(ORB_SLAM2::System* slam) {
    frameDrawer = slam->mpFrameDrawer;
}

int VisualDebugger::show(long deltaTime){

    if(!config->isVisualDebugEnabled() && !config->isOutputHudVideoEnabled() &&
            !config->isOutputRawVideoEnabled())
        return 0;

    if(frame.cols == 0 || frame.rows == 0)
        return 0;


    cv::Point center = cv::Point(frame.cols / 2, frame.rows / 2);

    cv::line(frame, cv::Point(center.x - 5, center.y), cv::Point(center.x + 5, center.y),WHITE_COLOR);
    cv::line(frame, cv::Point(center.x, center.y - 5), cv::Point(center.x, center.y + 5),WHITE_COLOR);

    double roiStart = DetectAndTrack::ROI_MARGIN;
    double roiEnd = 1 - DetectAndTrack::ROI_MARGIN;

    cv::rectangle(frame,
                  cv::Point((int)(frame.cols * roiStart), (int)(frame.rows * roiStart)),
                  cv::Point((int)(frame.cols * roiEnd), (int)(frame.rows * roiEnd)),
                  WHITE_COLOR);

    int consoleY = 10;
    bool first = true;

    for(std::string str : console){

        cv::Size textSize =
                cv::getTextSize(str, CONSOLE_FONT, CONSOLE_FONT_SIZE, CONSOLE_FONT_THICKNESS, NULL);

        cv::Point textOrigin = cv::Point(frame.cols - textSize.width - 10,
            consoleY + textSize.height);

        cv::putText(frame, str, textOrigin, CONSOLE_FONT, CONSOLE_FONT_SIZE, first ? WHITE_COLOR : GREY_COLOR,
                    CONSOLE_FONT_THICKNESS, cv::LINE_AA);

        consoleY += textSize.height + 10;
        first = false;

    }

    if(hudOutput.isOpened() || rawOutput.isOpened()) {

        double timePerFrame = 1000000 / OUTPUT_FPS;

        int framesPassed = config->isRealTimeVideoOutputEnabled() ? (int)(deltaTime / timePerFrame) : 1;

        if(framesPassed > OUTPUT_FPS)
            Logger::logWarning("%u frames passed") << framesPassed;

        for (int i = 0; i < framesPassed; ++i) {
            if(hudOutput.isOpened())
                hudOutput.write(frame);

            if(rawOutput.isOpened())
                rawOutput.write(*originalFrame);
        }

    }

    if(config->isVisualDebugEnabled() && frame.cols > 0 && frame.rows > 0) {
        cv::imshow(windowName, frame);
        return cv::waitKey(1);
    } else {
        return 0;
    }
}

void VisualDebugger::cleanup() {
    cvDestroyWindow(windowName.c_str());

    if(hudOutput.isOpened())
        hudOutput.release();

    if(rawOutput.isOpened())
        rawOutput.release();
}

void VisualDebugger::writeConsole(std::string str) {
    if(console.size() == CONSOLE_QUEUE_SIZE)
        console.pop_back();

    console.insert(console.begin(), str);
}

void VisualDebugger::setFollowCommand(FollowCommand command) {

    if(!config->isVisualDebugEnabled() && !config->isOutputHudVideoEnabled())
        return;

    if(frame.cols == 0 && frame.rows == 0)
        return;

    std::vector<Track>::iterator iterator = std::find_if(tracks.begin(), tracks.end(),
                             [this, command](Track t){return t.getNumber() == command.followee;});

    Track track = *iterator;

    cv::Scalar color =  colors[track.getNumber() % (sizeof(colors)/sizeof(cv::Scalar))];

    cv::Rect2d trackRect = track.getRect();
    rectangle(frame,
              cv::Rect2d(trackRect.x + 10, trackRect.y + 10, trackRect.width - 20, trackRect.height - 20),
              color, 1, 1);

    std::string str = (boost::format("%.2fm") % command.linearDisplacement.y).str();

    cv::Size textSize = cv::getTextSize(str, CONSOLE_FONT, 1, 1, NULL);
    cv::Point textOrigin = cv::Point((int)trackRect.x, (int)trackRect.y - textSize.height);
    cv::putText(frame, str, textOrigin, CONSOLE_FONT, 1, color, 1, cv::LINE_AA);

    cv::Point frameCenter = cv::Point(frame.cols / 2, frame.rows /2);
    cv::circle(frame, frameCenter, 40, RED_COLOR);

    double xPercentage = command.outputRotation.Yaw() / Follower::YAW_MAX_VELOCITY;
    double yPercentage = command.outputDisplacement.Pitch() / Follower::DISPLACEMENT_MAX_VELOCITIY;
    double length = std::sqrt(xPercentage * xPercentage + yPercentage * yPercentage);

    cv::Point displacement = cv::Point(frameCenter.x + xPercentage / length * 40,
                                       frameCenter.y - yPercentage / length * 40);

    cv::arrowedLine(frame, frameCenter, displacement, RED_COLOR);

}

void VisualDebugger::drawMouse(double deltaTime) {

    FollowCommand command = follower->getCommand(altitude, deltaTime, mousePosition);

    std::string str = (boost::format("%.2fm") % command.linearDisplacement.y).str();

    cv::Size textSize = cv::getTextSize(str, CONSOLE_FONT, 1, 1, NULL);
    cv::Point textOrigin = cv::Point(mousePosition.x + 10, mousePosition.y + 10);
    cv::putText(frame, str, textOrigin, CONSOLE_FONT, 1, VisualDebugger::WHITE_COLOR, 1, cv::LINE_AA);

}

void VisualDebugger::drawHorizon(int y) {

    cv::line(frame, cv::Point(0,y), cv::Point(frame.cols,y), RED_COLOR);

    std::string str = (boost::format("Tilt: %u") % config->getCameraTilt()).str();

    cv::Size textSize = cv::getTextSize(str, CONSOLE_FONT, 1, 1, NULL);
    cv::Point textOrigin = cv::Point(frame.cols - textSize.width - 10, y - textSize.height);
    cv::putText(frame, str, textOrigin, CONSOLE_FONT, 1, VisualDebugger::RED_COLOR, 1, cv::LINE_AA);

}
