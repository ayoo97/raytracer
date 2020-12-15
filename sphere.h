// some of the following header file is from https://raytracing.github.io/books/RayTracingInOneWeekend.html

#ifndef SPHERE_H
#define SPHERE_H

#include "hittable.h"
#include "vec3.h"

class sphere : public hittable {

    public:
        point3 center;
        double radius;
        shared_ptr<material> mat_ptr;

    public:
        sphere() {}
        sphere(point3 cen, double r, shared_ptr<material> m)
            : center(cen), radius(r), mat_ptr(m) {};

        // bool hit(const ray& r, double t_min, double t_max, hit_record& rec) const override
        // - takes in a ray, lower/upper bound, and a hit record
        // - determines whether something is hit during search and stores the point in space
        //   in which it was hit, the normal, and the root t
        bool hit(const ray& r, double t_min, double t_max, hit_record& rec) const override {
            vec3 oc = r.origin() - center;
            auto a = r.direction().length_squared();
            auto half_b = dot(oc, r.direction());
            auto c = oc.length_squared() - (radius * radius);

            auto discriminant = (half_b * half_b) - (a * c);
            if (discriminant < 0 ) return false;
            auto sqrtd = sqrt(discriminant);

            // Find the nearest root that lies in the acceptable range.
            auto root = (-half_b - sqrtd) / a;
            if (root < t_min || t_max < root) {
                root = (-half_b + sqrtd) / a;
                if (root < t_min || t_max < root) {
                    return false;
                }
            }

            rec.t = root;
            rec.p = r.at(rec.t);
            vec3 outward_normal = (rec.p - center) / radius;
            rec.set_face_normal(r, outward_normal);
            rec.mat_ptr = mat_ptr;

            return true;
        }

};

#endif