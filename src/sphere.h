#ifndef SPHERE_H
#define SPHERE_H

#include "hittable.h"
#include "vec3.h"

class sphere : public hittable {
public:
    // Stationary sphere
    sphere(point3 _center, double _radius, shared_ptr<material> _material) 
        : center1(_center), radius(_radius), mat(_material), is_moving(false) {}

    // Moving sphere
    sphere(point3 _center1, point3 _center2, double _radius, shared_ptr<material> _material) 
        : center1(_center1), radius(_radius), mat(_material), is_moving(true) {
            center_vec = _center2 - center1;
        }

    bool hit(const ray &r, interval ray_t, hit_record &rec) const override {
        point3 center = is_moving? sphere_center(r.time()) : center1;
        vec3 oc = r.origin() - center;  // A-C
        double a = r.direction().length_squared();  // b^2
        double h = dot(r.direction(), oc); // b.(A-C)
        double c = oc.length_squared() - radius*radius;

        double discriminant = h*h - a*c;
        if (discriminant < 0) return false;  // no solutions
        double sqrtd = sqrt(discriminant);

        // find the closest solution/root, t, to the camera that lies in the acceptable range
        double root = (-h - sqrtd) / a;
        if (!ray_t.surrounds(root)) {  // closest (smallest) root is out of range
            root = (-h + sqrtd) / a;
            if (!ray_t.surrounds(root)) { // largest root is also out of range
                return false;
            }
        }

        rec.t = root;
        rec.p = r.at(rec.t);  // the point of collision
        vec3 outward_normal = (rec.p - center) / radius;
        rec.set_face_normal(r, outward_normal);
        rec.mat = mat;

        return true;
    }

private:
    point3 center1;
    double radius;
    shared_ptr<material> mat;
    bool is_moving;
    vec3 center_vec;  // delta vector between (from) center1 and (to) center2

    point3 sphere_center(double time) const {
        // linearly interpolate from center1 to center2 
        // s.t. center(t=0) = center1, center(t=1) = center2
        return center1 + time * center_vec;
    }
};

#endif