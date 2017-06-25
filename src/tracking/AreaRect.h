//
// Created by santy on 23/05/17.
//

#ifndef PROY_GRADO_AREARECT_H
#define PROY_GRADO_AREARECT_H


struct AreaRect {

    AreaRect(){}
    AreaRect(cv::Rect r, bool isPreviousTrack)
    {
        x1 = r.x;
        y1 = r.y;
        x2 = r.x + r.width;
        y2 = r.y + r.height;
        area = r.width * r.height;
        rect = r;
        this->isPreviousTrack = isPreviousTrack;
    }
    double x1, x2, y1, y2, area;

    cv::Rect rect;
    bool isPreviousTrack;

};

#endif //PROY_GRADO_AREARECT_H
