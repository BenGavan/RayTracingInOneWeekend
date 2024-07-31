#ifndef QUAD_H
#define QUAD_H

#include "rtweekend.h"
#include "hittable.h"
#include "hittable_list.h"

#include <cmath>

// parallelogram
class quad : public hittable {
public:
    quad(const point3& _Q, const vec3& _u, const vec3& _v, shared_ptr<material> m) 
            : Q(_Q), u(_u), v(_v), mat(m) {
        vec3 n = cross(u, v);
        normal = unit_vector(n);
        D = dot(normal, Q);
        w = n / dot(n,n); // \hat{n} / n

        set_bounding_box();
    }

    virtual void set_bounding_box() {
        bbox = aabb(Q, Q + u +v).pad();
    }

    aabb bounding_box() const override { return bbox; }

    bool hit(const ray& r, interval ray_t, hit_record& rec) const override {
        double denom = dot(normal, r.direction());

        // No hit if the ray is parallel to the plane
        if (fabs(denom) < 1e-8) return false;

        // No hit if the hit point parameter, t, is outside the ray interval
        double t = (D - dot(normal, r.origin())) / denom;
        if (!ray_t.contains(t)) return false;

        // Determine the hit point lies within the planar shape using its plane coordinates
        point3 intersection = r.at(t);
        vec3 planar_hitpt_vector = intersection - Q;
        double alpha = dot(w, cross(planar_hitpt_vector, v));
        double beta = dot(w, cross(u, planar_hitpt_vector));

        if (!is_interior(alpha, beta, rec)) return false;
        // Ray hits shape 

        rec.t = t;
        rec.p = intersection;
        rec.mat = mat;
        rec.set_face_normal(r, normal);
        
        return true;
    }

    virtual bool is_interior(double a, double b, hit_record& rec) const {
        // Given the hit point in plane coords, return false if it is outide 
        // the primitive, otherwise set the hit record UV coords and return true

        if ((a < 0) || (1 < a) || (b < 0) || (1 < b)) return false;

        rec.u = a;
        rec.v = b;
        return true;
    }

private:
    point3 Q;  // origin of the quad/parallelogram
    vec3 u, v; // the two sides originating from Q (The parallelogram has 4 vertices: Q, Q+u, Q+v, Q+u+v) (Q and Q+u+v are oposite corners)
    shared_ptr<material> mat;
    aabb bbox;
    vec3 normal;
    double D;
    vec3 w;
};

inline shared_ptr<hittable_list> box(const point3& a, const point3& b, shared_ptr<material> mat) {
    // Returns the 3D box (six sides) that contain the two opposite vertices a & b 

    auto sides = make_shared<hittable_list>();

    // Two opposite vertices with the minimum and maximum coords
    auto min = point3(fmin(a.x(), b.x()), fmin(a.y(), b.y()), fmin(a.z(), b.z()));
    auto max = point3(fmax(a.x(), b.x()), fmax(a.y(), b.y()), fmax(a.z(), b.z()));

    auto dx = vec3(max.x() - min.x(), 0, 0);
    auto dy = vec3(0, max.y() - min.y(), 0);
    auto dz = vec3(0, 0, max.z() - min.z());

    sides->add(make_shared<quad>(point3(min.x(), min.y(), max.z()), dx, dy, mat));  // front
    sides->add(make_shared<quad>(point3(max.x(), min.y(), max.z()), -dz, dy, mat));  // right
    sides->add(make_shared<quad>(point3(max.x(), min.y(), min.z()), -dx, dy, mat));  // back
    sides->add(make_shared<quad>(point3(min.x(), min.y(), min.z()), dz, dy, mat));  // left
    sides->add(make_shared<quad>(point3(min.x(), max.y(), max.z()), dx, -dz, mat));  // top
    sides->add(make_shared<quad>(point3(min.x(), min.y(), min.z()), dx, dz, mat));  // bottom

    return sides;
}

#endif // QUAD_H