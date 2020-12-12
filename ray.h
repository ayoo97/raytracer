// some of the following header file is from https://raytracing.github.io/books/RayTracingInOneWeekend.html

#ifndef RAY_H
#define RAY_H

#include "vec3.h"

class ray {

    public:
        // MEMBERS
        point3 orig;
        vec3 dir;

    public:
        // CONSTRUCTORS
        ray() {}
        ray(const point3& origin, const vec3& direction)
            : orig(origin), dir(direction) {}

        // GETTER METHODS
        point3 origin() const { return orig; }
        vec3 direction() const { return dir; }

        point3 at(double t) const {
            return orig + (t * dir);
        }

};

#endif