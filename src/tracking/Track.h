//
// Created by bruno on 03/04/17.
//

#ifndef PROY_GRADO_TRACK_H
#define PROY_GRADO_TRACK_H


#include <opencv2/core/types.hpp>

class Track {
public:
    Track();
    Track(int number, cv::Rect2d rect);
    int getNumber();
    void setNumber(int number);
    cv::Rect2d getRect();
    void setRect(cv::Rect2d r);
    double getOverlapRatio(cv::Rect2d r) const;
private:
    int number;
    cv::Rect2d rect;
};


#endif //PROY_GRADO_TRACK_H
