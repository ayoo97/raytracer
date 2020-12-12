// some of the following header file is from https://raytracing.github.io/books/RayTracingInOneWeekend.html

#ifndef RTWEEKEND_H
#define RTWEEKEND_H

#include <cmath>
#include <limits>
#include <memory>
#include <cstdlib>

using namespace std;

// Constants

const double infinity = numeric_limits<double>::infinity();
const double pi = 3.1415926535897932385;

// Utility Functions

const inline double degrees_to_radians(double degrees) {
    return (degrees * pi) / 180.0;
}

inline double random_double() {
    // Return a random real in (0, 1]
    return rand() / (RAND_MAX + 1.0);
}

inline double random_double(double min, double max) {
    // Returns a random real in [min, max)
    return min + ((max - min) * random_double());
}

// inline double clamp(double x, double min, double max)
// - clamps value x to either the min or the max (whichever is closest)
inline double clamp(double x, double min, double max) {
    if (x < min) return min;
    if (x > max) return max;
    return x;
}

// Common Headers

#include "ray.h"
#include "vec3.h"

#endif