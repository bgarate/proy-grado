//
// Created by bruno on 11/06/17.
//

#ifndef PROY_GRADO_POINT_H
#define PROY_GRADO_POINT_H

//Definicion point
typedef struct Point {
    double x;
    double y;
    double z;

    Point(double x, double y, double z) {
        this->x = x;
        this->y = y;
        this->z = z;
    }

    Point() {
        x = 0;
        y = 0;
        z = 0;
    }

    double Roll() { return x; }
    void Roll(double x) { this->x = x; }

    double Pitch() { return y; }
    void Pitch(double y) { this->y = y; }

    double Yaw() { return z; }
    void Yaw(double z) { this->z = z; }

    double Latitude() { return x; }

    double Longitude() { return y; }

    double Altitude() { return z; }

    double Pan() { return x;}

    double Tilt() { return  y;}

} Point;

#endif //PROY_GRADO_POINT_H
