// the following header file is from https://raytracing.github.io/books/RayTracingInOneWeekend.html

#ifndef HITTABLE_H
#define HITTABLE_H

#include "ray.h"

struct hit_record {
    point3 p;
    vec3 normal;
    double t;
    bool front_face;

    // set_face_normal(const ray& r, const vec3& outward_normal)
    // - takes in a ray and the outward_normal and determines whether
    //   the ray is front-facing (against the normal)
    // - then sets the normal to negative or positive outward_normal
    inline void set_face_normal(const ray&r, const vec3& outward_normal) {
        front_face = dot(r.direction(), outward_normal) < 0;
        normal = front_face ? outward_normal : -outward_normal;
    }
};

class hittable {
    public:
        virtual bool hit(const ray& r, double t_min, double t_max, hit_record& rec) const = 0;
};

#endif