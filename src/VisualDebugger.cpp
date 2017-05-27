//
// Created by bruno on 27/05/17.
//

#include <boost/format.hpp>
#include "VisualDebugger.h"

const cv::Scalar VisualDebugger::WHITE_COLOR = cv::Scalar(255,255,255);
const cv::Scalar VisualDebugger::GREEN_COLOR = cv::Scalar(0,205,0);

void VisualDebugger::setup(Config *config) {
    this->config = config;
    windowName = config->getName() + " - VisualDebugger";

    if(config->isVisualDebugEnabled())
        cv::namedWindow(windowName, cv::WINDOW_AUTOSIZE);
}

void VisualDebugger::setFrame(std::shared_ptr<cv::Mat> frame) {
    if(frame != NULL)
        frame->copyTo(this->frame);
}

void VisualDebugger::setTracks(std::vector<Track> tracks) {

    if(!config->isVisualDebugEnabled())
        return;

    // draw the tracked object
    for (unsigned i = 0; i < tracks.size(); i++)
        rectangle(frame, tracks[i].getRect(),
                  colors[tracks[i].getNumber() % (sizeof(colors)/sizeof(cv::Scalar))], 2, 1);

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

void VisualDebugger::setStatus(State state, int battery, double altitude, Point gps, Point orientation){

    std::string statusName = getStateName(state);
    cv::Scalar statusColor =  getStateColor(state);

    cv::Size textSize =
            cv::getTextSize(statusName, CONSOLE_FONT, 2, 1, NULL);

    cv::Point textOrigin = cv::Point(10, textSize.height + 10);

    cv::putText(frame, statusName, textOrigin, CONSOLE_FONT, 2, statusColor, 1);

    std::vector<std::string> colValues = {
            (boost::format("Bateria: %3u%%") % battery).str(),
            (boost::format("Altitud: %3.2fm") % altitude).str(),
            (boost::format("Lat: %3.2f Long: %3.2f Alt: %3.2f") % orientation.Latitude() %
             orientation.Longitude() % orientation.Altitude()).str(),
            (boost::format("Roll: %3.2f Pitch: %3.2f Yaw: %3.2f") % orientation.Roll() %
            orientation.Pitch() % orientation.Yaw()).str(),
    };


    textOrigin = cv::Point(10, frame.rows - 10);
    cv::putText(frame, colValues[0], textOrigin, CONSOLE_FONT, 1, GREEN_COLOR);


    textSize = cv::getTextSize(colValues[1], CONSOLE_FONT, 1, 1, NULL);
    textOrigin = cv::Point((frame.cols - textSize.width)/2, frame.rows - 10);
    cv::putText(frame, colValues[1], textOrigin, CONSOLE_FONT, 1, GREEN_COLOR);

    textSize = cv::getTextSize(colValues[2], CONSOLE_FONT, 1, 1, NULL);
    textOrigin = cv::Point(frame.cols - textSize.width - 10, frame.rows - 10);
    cv::putText(frame, colValues[2], textOrigin, CONSOLE_FONT, 1, GREEN_COLOR);

    cv::Size previousTextSize = textSize;
    textSize = cv::getTextSize(colValues[3], CONSOLE_FONT, 1, 1, NULL);
    textOrigin = cv::Point(frame.cols - textSize.width - 10, frame.rows - previousTextSize.height - 20);
    cv::putText(frame, colValues[3], textOrigin, CONSOLE_FONT, 1, GREEN_COLOR);

}

int VisualDebugger::show(){

    if(!config->isVisualDebugEnabled())
        return 0;


    if(frame.cols == 0 || frame.rows == 0)
        return 0;


    cv::Point center = cv::Point(frame.cols / 2, frame.rows / 2);

    cv::line(frame, cv::Point(center.x - 5, center.y), cv::Point(center.x + 5, center.y),WHITE_COLOR);
    cv::line(frame, cv::Point(center.x, center.y - 5), cv::Point(center.x, center.y + 5),WHITE_COLOR);

    int consoleY = 10;

    for(std::string str : console){

        cv::Size textSize =
                cv::getTextSize(str, CONSOLE_FONT, CONSOLE_FONT_SIZE, CONSOLE_FONT_THICKNESS, NULL);

        cv::Point textOrigin = cv::Point(frame.cols - textSize.width - 10,
            consoleY + textSize.height);

        cv::putText(frame, str, textOrigin, CONSOLE_FONT, CONSOLE_FONT_SIZE, WHITE_COLOR, CONSOLE_FONT_THICKNESS);

        consoleY += textSize.height + 10;

    }

    cv::imshow(windowName, frame);
    return cv::waitKey(1);
}

void VisualDebugger::cleanup() {
    cvDestroyWindow(windowName.c_str());
}

void VisualDebugger::writeConsole(std::string str) {
    if(console.size() == CONSOLE_QUEUE_SIZE)
        console.pop_back();

    console.insert(console.begin(), str);
}
