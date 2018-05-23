//
// Created by bruno on 03/04/17.
//

#include "Track.h"

Track::Track() {
}

int Track::getNumber() {
    return number;
}

void Track::setNumber(int number) {
    this->number = number;
}

cv::Rect2d Track::getRect() {
    return rect;
}

void Track::setRect(cv::Rect2d r) {
    rect = cv::Rect2d(r);
}

double Track::getOverlapRatio(cv::Rect2d r) const {
    double xx1 = std::max(rect.x, r.x);
    double yy1 = std::max(rect.y, r.y);
    double xx2 = std::min(rect.x + rect.width, r.x + r.width);
    double yy2 = std::min(rect.y + rect.height, r.y + r.height);

    double width = std::max(0.0, xx2 - xx1 + 1);
    double heigth = std::max(0.0, yy2 - yy1 + 1);

    return (width*heigth)/(rect.width * rect.height);
}

Track::Track(int number, cv::Rect2d rect) {
    this->number = number;
    this->rect = rect;
}
