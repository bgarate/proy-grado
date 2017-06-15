//
// Created by bruno on 27/05/17.
//

#include <boost/format.hpp>
#include <chrono>
#include <src/logging/Logger.h>
#include <src/tracking/DetectAndTrack.h>
#include "VisualDebugger.h"
#include <src/hal/Point.h>

const cv::Scalar VisualDebugger::WHITE_COLOR = cv::Scalar(255,255,255);
const cv::Scalar VisualDebugger::GREEN_COLOR = cv::Scalar(0,205,0);
const cv::Scalar VisualDebugger::GREY_COLOR = cv::Scalar(205,205,205);

void VisualDebugger::setup(Config *config) {
    this->config = config;
    windowName = config->getName() + " - VisualDebugger";

    if(config->isVisualDebugEnabled())
        cv::namedWindow(windowName, cv::WINDOW_AUTOSIZE);

    shouldOpen = config->isOutputRawVideoEnabled() || config->isOutputHudVideoEnabled();

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
        cv::putText(frame, number, textOrigin, CONSOLE_FONT, 1, color);
    }

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

    std::string statusName = getStateName(state);
    cv::Scalar statusColor =  getStateColor(state);

    cv::Size textSize =
            cv::getTextSize(statusName, CONSOLE_FONT, 2, 1, NULL);

    cv::Point textOrigin = cv::Point(10, textSize.height + 10);

    cv::putText(frame, statusName, textOrigin, CONSOLE_FONT, 2, statusColor, 1);

    long runningSeconds = runningTime / 1000000;
    long runningMinutes = runningSeconds / 60;

    std::vector<std::string> colValues = {
            (boost::format("FPS: %3u Running time: %02um:%02us") % fps % runningMinutes % (runningSeconds % 60)).str(),
            (boost::format("Bateria: %3u%%") % battery).str(),
            (boost::format("Altitud: %3.2fm") % altitude).str(),
            (boost::format("Lat: %3.2f Long: %3.2f Alt: %3.2f") % orientation.Latitude() %
             orientation.Longitude() % orientation.Altitude()).str(),
            (boost::format("Roll: %3.2f Pitch: %3.2f Yaw: %3.2f") % orientation.Roll() %
            orientation.Pitch() % orientation.Yaw()).str(),
    };

    textSize = cv::getTextSize(colValues[0], CONSOLE_FONT, 1, 1, NULL);
    textOrigin = cv::Point((frame.cols - textSize.width)/2, textSize.height + 10);
    cv::putText(frame, colValues[0], textOrigin, CONSOLE_FONT, 1, GREEN_COLOR);

    textOrigin = cv::Point(10, frame.rows - 10);
    cv::putText(frame, colValues[1], textOrigin, CONSOLE_FONT, 1, GREEN_COLOR);

    textSize = cv::getTextSize(colValues[2], CONSOLE_FONT, 1, 1, NULL);
    textOrigin = cv::Point((frame.cols - textSize.width)/2, frame.rows - 10);
    cv::putText(frame, colValues[2], textOrigin, CONSOLE_FONT, 1, GREEN_COLOR);

    textSize = cv::getTextSize(colValues[3], CONSOLE_FONT, 1, 1, NULL);
    textOrigin = cv::Point(frame.cols - textSize.width - 10, frame.rows - 10);
    cv::putText(frame, colValues[3], textOrigin, CONSOLE_FONT, 1, GREEN_COLOR);

    cv::Size previousTextSize = textSize;
    textSize = cv::getTextSize(colValues[4], CONSOLE_FONT, 1, 1, NULL);
    textOrigin = cv::Point(frame.cols - textSize.width - 10, frame.rows - previousTextSize.height - 20);
    cv::putText(frame, colValues[4], textOrigin, CONSOLE_FONT, 1, GREEN_COLOR);

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
                    CONSOLE_FONT_THICKNESS);

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
