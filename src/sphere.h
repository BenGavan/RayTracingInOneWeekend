#ifndef SPHERE_H
#define SPHERE_H

#include "hittable.h"
#include "vec3.h"

class sphere : public hittable {
public:
    sphere(point3 _center, double _radius) : center(_center), radius(_radius) {}

    bool hit(const ray &r, double ray_t_min, double ray_t_max, hit_record &rec) const override {
        vec3 oc = r.origin() - center;  // A-C
        double a = r.direction().length_squared();  // b^2
        double h = dot(r.direction(), oc); // b.(A-C)
        double c = oc.length_squared() - radius*radius;

        double discriminant = h*h - a*c;
        if (discriminant < 0) return false;  // no solutions
        double sqrtd = sqrt(discriminant);

        // find the closest solution/root, t, to the camera that lies in the acceptable range
        double root = (-h - sqrtd) / a;
        if (root <= ray_t_min || root >= ray_t_max) {  // closest (smallest) root is out of range
            root = (-h + sqrtd) / a;
            if (root <= ray_t_min || root >= ray_t_max) { // largest root is also out of range
                return false;
            }
        }

        rec.t = root;
        rec.p = r.at(rec.t);  // the point of collision
        vec3 outward_normal = (rec.p - center) / radius;
        rec.set_face_normal(r, outward_normal);
        return true;
    }

private:
    point3 center;
    double radius;
};

#endif