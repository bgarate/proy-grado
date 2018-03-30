//
// Created by bruno on 25/07/17.
//

#ifndef PROY_GRADO_HELPER_H
#define PROY_GRADO_HELPER_H

#include <opencv2/core/types.hpp>

class Helpers {
public:
    template <typename T>
    static T Clamp(const T& value, const T& low, const T& high)
    {
      return value < low ? low : (value > high ? high : value);
    }

    template <typename T>
    static cv::Rect_<T> Clip(const cv::Rect_<T>& value, const cv::Size_<T>& size)
    {
      cv::Rect_<T> r(value);
      r.x = Clamp(r.x, 0, size.width);
      r.y = Clamp(r.y, 0, size.height);
      r.width = Clamp(r.width, 0, size.width - r.x);
      r.height = Clamp(r.height, 0, size.height- r.y);

      return r;
    }

    static double angleDifference(double target, double origin){
        double  diff = target - origin;
        diff = signedMod((diff + 180),360.0) - 180;

        return diff;
    }

    static double signedMod(double a, double n) {
        return a - std::floor(a/n) * n;
    }

    static cv::Vec2d rotate(cv::Vec2d v, double angle)
    {
        return cv::Vec2d(v[0] * std::cos(angle) - v[1] * std::sin(angle),
                         v[0] * std::sin(angle) + v[1] * std::cos(angle));
    }


    static double toDegrees(double rad) {
        return rad / M_PI * 180;
    }

    static double toRadians(double deg) {
        return deg / 180 * M_PI;
    }

};


#endif //PROY_GRADO_HELPER_H
