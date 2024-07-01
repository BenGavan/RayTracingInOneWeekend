#include "rtweekend.h"
#include "camera.h"
#include "color.h"
#include "hittable_list.h"
#include "material.h"
#include "sphere.h"

int main() {
    hittable_list world;

    double R = cos(pi/4.0);

    // auto material_ground = make_shared<lambertian>(color(0.8, 0.8, 0.0));
    // auto material_center = make_shared<lambertian>(color(0.1, 0.2, 0.5));
    auto material_left   = make_shared<lambertian>(color(0,0,1));
    auto material_right  = make_shared<lambertian>(color(1,0,0));

    // world.add(make_shared<sphere>(point3( 0.0, -100.5, -1.0), 100, material_ground));
    // world.add(make_shared<sphere>(point3( 0.0,    0.0, -1.0), 0.5, material_center));
    // world.add(make_shared<sphere>(point3(-1.2,    0.0, -1.0), 0.5, material_left));
    world.add(make_shared<sphere>(point3(-R, 0.0, -1.0), R, material_left));  // Negative radius leaves geometry the same, but makes the surface normal point inwards (used to make a hollow glass sphere)
    world.add(make_shared<sphere>(point3( R, 0.0, -1.0), R, material_right));

    camera cam;
    cam.aspect_ratio      = 16.0 / 9.0;
    cam.image_width       = 400;
    cam.samples_per_pixel = 100;
    cam.max_depth         = 50;
    cam.vfov              = 90;

    cam.render(world);

    return 0;
}