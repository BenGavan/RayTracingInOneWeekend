#ifndef CONSTANT_MEDIUM_H
#define CONSTANT_MEDIUM_H

#include "rtweekend.h"
#include "hittable.h"
#include "material.h"
#include "texture.h"

class constant_medium : public hittable {
public:

    constant_medium(shared_ptr<hittable> b, double d, shared_ptr<texture> a) 
        : boundary(b), neg_inv_denisty(-1/d), phase_function(make_shared<isotropic>(a)) {}

    constant_medium(shared_ptr<hittable> b, double d, color c) 
        : boundary(b), neg_inv_denisty(-1/d), phase_function(make_shared<isotropic>(c)) {}
    
    bool hit(const ray& r, interval ray_t, hit_record& rec) const override {
        // print occasional samples when debugging.  To enable, set enableDebug true
        const bool enableDebug = false;
        const bool debugging = enableDebug && random_double() < 0.00001;

        hit_record rec1, rec2;

        if (!boundary->hit(r, universe, rec1)) return false;

        if (!boundary->hit(r, interval(rec1.t+0.0001, infinity), rec2)) return false;

        if (debugging) std::clog << "\n ray_tmin=" << rec1.t << ", ray_tmax=" << rec2.t << '\n';

        if (rec1.t < ray_t.min) rec1.t = ray_t.min;
        if (rec2.t > ray_t.max) rec2.t = ray_t.max;

        if (rec1.t >= rec2.t) return false;

        if (rec1.t < 0) rec1.t = 0;

        double ray_length = r.direction().length();
        double distance_inside_boundary = (rec2.t - rec1.t) * ray_length;
        double hit_distance = neg_inv_denisty * log(random_double());
        
        if (hit_distance > distance_inside_boundary) return false;

        rec1.t = rec1.t + hit_distance / ray_length;
        rec.p = r.at(rec.t);

        if (debugging) {
            std::clog << "hit_distance = " << hit_distance << '\n'
                      << "rec.t = " << rec.t << '\n'
                      << "rec.p = " << rec.p << '\n';
        }


        rec.normal = vec3(1,0,0);  // arbitrary
        rec.front_face = true;     // arbitrary
        rec.mat = phase_function;

        return true;
    }

    aabb bounding_box() const override { return boundary-> bounding_box(); }

private:
    shared_ptr<hittable> boundary;
    double neg_inv_denisty;
    shared_ptr<material> phase_function;
};


#endif // CONSTANT_MEDIUM_H