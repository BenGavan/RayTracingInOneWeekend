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
    double aspect_ratio      = 10;   // Ratio of image width over height
    int    image_width       = 100;  // Rendered image width in pixels
    int    samples_per_pixel = 10;   // Number of rays (samples) sent out per pixel
    int    max_depth         = 10;   // Max number of times a single ray can bounce within the scene 

    void render(const hittable &world) {
        initialize();

        std::cout << "P3\n";  // P3 := colors are in ASCII
        std::cout << image_width << ' ' << image_height << '\n'; // Image width & height (i.e. # columns and rows)
        std::cout << "255\n"; // 255 := Max color

        for (int j=0; j < image_height; ++j) {  // write pixels out in rows
            std::clog << "\rScanlines remaining: " << (image_height - j) << ' ' << std::flush;
            for (int i=0; i<image_width; ++i) {  // each row is written out left to right
                color pixel_color(0,0,0);
                for (int sample = 0; sample < samples_per_pixel; ++sample) {
                    ray r = get_ray(i, j);
                    pixel_color += ray_color(r, max_depth, world);
                }
                write_color(std::cout, pixel_color, samples_per_pixel);
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

    ray get_ray(int i, int j) const {
        // Randomly sampled camera ray for the pixel at location i, j
        point3 pixel_center = pixel00_loc + (i * pixel_delta_u) + (j * pixel_delta_v);
        point3 pixel_sample = pixel_center + pixel_sample_square();

        vec3 ray_origin = center;
        vec3 ray_direction = pixel_sample - ray_origin;

        return ray(ray_origin, ray_direction);
    }

    vec3 pixel_sample_square() const {
        // Returns a random point in the square surrouding the pixel at the origin
        double px = -0.5 + random_double();  // random double \in [-0.5, 0.5)
        double py = -0.5 + random_double();  // random double \in [-0.5, 0.5)
        return (px * pixel_delta_u) + (py * pixel_delta_v);
    }

    color ray_color(const ray &r, int depth_remaining, const hittable &world) const {
        hit_record rec;

        if (depth_remaining <= 0) {
            return color(0,0,0);  // this ray has been scattered so many times, it can be considered to be black (limit inplave to stop stack blowing up)
        }

        if (world.hit(r, interval(0.001, infinity), rec)) {  // lower bound of 0.001 to ignore second intersections of reflected rays that have been floating point errored to be within the sphere. (Reduces the shadow acne problem)
            vec3 direction = rec.normal + random_unit_vector();
            return 0.5 * ray_color(ray(rec.p, direction), depth_remaining-1, world);
        }

        vec3 unit_direction = unit_vector(r.direction());
        double a = 0.5 * (unit_direction.y() + 1.0);
        return (1.0-a) * color(1,1,1) + a * color(0.5, 0.7, 1.0);
    }

};

#endif