#include "color.h"
#include "ray.h"
#include "vec3.h"

#include <iostream>

bool hit_sphere(const point3 &center, double radius, const ray &r) {
    vec3 oc = r.origin() - center;  // A - C
    double a = dot(r.direction(), r.direction()); // b^2  (b from P(t) = A + t*b)
    double b = 2.0 * dot(oc, r.direction());  // 2(A-C).b
    double c = dot(oc, oc) - radius*radius;
    double discriminant = b*b - 4.0*a*c;
    return (discriminant >= 0); // 1 or more solutions
}

color ray_color(const ray& r) {
    if (hit_sphere(point3(0,0,-1), 0.5, r))
        return color(1,0,0);

    vec3 unit_direction = unit_vector(r.direction());
    double a = 0.5 * (unit_direction.y() + 1.0);
    return (1.0-a) * color(1,1,1) + a * color(0.5, 0.7, 1.0);
}

int main() {
    // Image
    double aspect_ratio = 16.0 / 9.0;
    int image_width = 256;
    int image_height = static_cast<int>(image_width / aspect_ratio);
    image_height = (image_height < 1) ? 1 : image_height;

    // Camera
    double focal_length = 1.0;
    double viewport_height = 2.0;
    double viewport_width = viewport_height * (static_cast<double>(image_width)/image_height);  // viewport are real, so can be less than one and non-integer
    point3 camera_center = point3(0,0,0);

    // vectors accross the horizontal and down the vertical viewport edges
    vec3 viewport_u = vec3(viewport_width, 0, 0);
    vec3 viewport_v = vec3(0, -viewport_height, 0);

    // pixel pitch vectors
    vec3 pixel_delta_u = viewport_u / image_width;
    vec3 pixel_delta_v = viewport_v / image_height;

    // location of the upper left pixel
    vec3 viewport_upper_left = camera_center - vec3(0,0,focal_length) - viewport_u/2 - viewport_v/2;
    vec3 pixel00_loc = viewport_upper_left + 0.5 * (pixel_delta_u + pixel_delta_v);

    // render
    std::cout << "P3\n";  // P3 := colors are in ASCII
    std::cout << image_width << ' ' << image_height << '\n'; // Image width & height (i.e. # columns and rows)
    std::cout << "255\n"; // 255 := Max color

    for (int j=0; j < image_height; ++j) {  // write pixels out in rows
        std::clog << "\rScanlines remaining: " << (image_height - j) << ' ' << std::flush;
        for (int i=0; i<image_width; ++i) {  // each row is written out left to right
            vec3 pixel_center = pixel00_loc + (i * pixel_delta_u) + (j * pixel_delta_v);
            vec3 ray_direction = pixel_center - camera_center;
            ray r(camera_center, ray_direction);

            color pixel_color = ray_color(r);
            write_color(std::cout, pixel_color);
        }
    }

    std::clog << "\rDone :)                \n";

    return 0;
}