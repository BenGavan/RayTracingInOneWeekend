/*
Will be responsible for:
 1. Construct and dispatch rays into the world
 2. Use the results of these rays to contruct the rendered image
*/
#ifndef CAMERA_H
#define CAMERA_H

#include "rtweekend.h"

#include "color.h"
#include "hittable.h"

#include <iostream>

class camera{
public:
    double aspect_ratio = 10;  // Ratio of image width over height
    int    image_width  = 100; // Rendered image width in pixels

    void render(const hittable &world) {
        initialize();

        std::cout << "P3\n";  // P3 := colors are in ASCII
        std::cout << image_width << ' ' << image_height << '\n'; // Image width & height (i.e. # columns and rows)
        std::cout << "255\n"; // 255 := Max color

        for (int j=0; j < image_height; ++j) {  // write pixels out in rows
            std::clog << "\rScanlines remaining: " << (image_height - j) << ' ' << std::flush;
            for (int i=0; i<image_width; ++i) {  // each row is written out left to right
                vec3 pixel_center = pixel00_loc + (i * pixel_delta_u) + (j * pixel_delta_v);
                vec3 ray_direction = pixel_center - center;
                ray r(center, ray_direction);

                color pixel_color = ray_color(r, world);
                write_color(std::cout, pixel_color);
            }
        }

        std::clog << "\rDone :)                \n";
    }
private:
    int image_height;   // Rendered image height
    point3 center;      // Camera center
    point3 pixel00_loc; // Location of pixel (0,0)
    vec3 pixel_delta_u; // Offset to pixel to the right (pixel pitch horizontally)
    vec3 pixel_delta_v; // Offset to pixel below (pixel pitch vertically)

    void initialize() {
        image_height = static_cast<int>(image_width / aspect_ratio);
        image_height = (image_height < 1) ? 1 : image_height;

        center = point3(0,0,0);

        // Camera - Determine viewport dimensions
        double focal_length = 1.0;
        double viewport_height = 2.0;
        double viewport_width = viewport_height * (static_cast<double>(image_width)/image_height);  // viewport are real, so can be less than one and non-integer

        // vectors accross the horizontal and down the vertical viewport edges
        vec3 viewport_u = vec3(viewport_width, 0, 0);
        vec3 viewport_v = vec3(0, -viewport_height, 0);

        // pixel pitch vectors
        pixel_delta_u = viewport_u / image_width;
        pixel_delta_v = viewport_v / image_height;

        // location of the upper left pixel
        vec3 viewport_upper_left = center - vec3(0,0,focal_length) - viewport_u/2 - viewport_v/2;
        pixel00_loc = viewport_upper_left + 0.5 * (pixel_delta_u + pixel_delta_v);
    }

    color ray_color(const ray &r, const hittable &world) const {
        hit_record rec;
        if (world.hit(r, interval(0, infinity), rec)) {
            return 0.5 * (rec.normal + color(1,1,1));
        }

        vec3 unit_direction = unit_vector(r.direction());
        double a = 0.5 * (unit_direction.y() + 1.0);
        return (1.0-a) * color(1,1,1) + a * color(0.5, 0.7, 1.0);
    }

};

#endif