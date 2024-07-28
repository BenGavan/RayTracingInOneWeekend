#ifndef QUAD_H
#define QUAD_H

#include "rtweekend.h"
#include "hittable.h"

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

#endif // QUAD_H