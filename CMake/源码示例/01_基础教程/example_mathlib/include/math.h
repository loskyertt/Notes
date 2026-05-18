#pragma once
#include <cmath>

inline double square(double x) {
    return x * x;
}

inline double distance(double x1, double y1, double x2, double y2) {
    return sqrt(square(x2 - x1) + square(y2 - y1));
}
