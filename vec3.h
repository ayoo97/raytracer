// the following header file is from https://raytracing.github.io/books/RayTracingInOneWeekend.html

#ifndef VEC3_H
#define VEC3_H

#include <cmath>
#include <iostream>

using namespace std;

class vec3 {
    
    public:
        double e[3];

    public:
        vec3() : e{ 0, 0, 0 } {}
        vec3(double e0, double e1, double e2) : e{ e0, e1, e2 } {}

        double x() const { return e[0]; }
        double y() const { return e[1]; }
        double z() const { return e[2]; }

        vec3 operator-() const { return vec3(-e[0], -e[1], -e[2]); }
        double operator[](int i) const { return e[i]; }
        double& operator[](int i) { return e[i]; }

        vec3& operator+=(const vec3 &v) {
            e[0] += v.e[0];
            e[1] += v.e[1];
            e[2] += v.e[2];
            return *this;
        }

        vec3& operator*=(const double t) {
            e[0] *= t;
            e[1] *= t;
            e[2] *= t;
            return *this;
        }

        vec3& operator/=(const double t) {
            return *this *= 1 / t;
        }

        double length_squared() const {
            return (e[0] * e[0]) + (e[1] * e[1]) + (e[2] * e[2]);
        }

        double length() const {
            return sqrt(length_squared());
        }
};

// Type aliases for vec3
using point3 = vec3;    // 3D Point
using color = vec3;     // RGB Color

#endif