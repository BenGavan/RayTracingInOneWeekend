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
#include "material.h"

#include <iostream>

class camera{
public:
    double aspect_ratio      = 10;   // Ratio of image width over height
    int    image_width       = 100;  // Rendered image width in pixels
    int    samples_per_pixel = 10;   // Number of rays (samples) sent out per pixel
    int    max_depth         = 10;   // Max number of times a single ray can bounce within the scene 
    color  background;               // Scene background color

    double vfov     = 90;              // Vertical view angle (field of view) (degrees)
    point3 lookfrom = point3(0,0,-1);  // Where the camera is looking from (where the sensor plane is?)
    point3 lookat   = point3(0,0,0);
    vec3 vup        = vec3(0,1,0);

    double defocus_angle = 0; // Variation angle to rays through each pixel
    double focus_dist = 10;   // Distance from camera lookfrom to perfect focus plane

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
    int    image_height;    // Rendered image height
    point3 center;       // Camera center
    point3 pixel00_loc;  // Location of pixel (0,0)
    vec3   pixel_delta_u;  // Offset to pixel to the right (pixel pitch horizontally)
    vec3   pixel_delta_v;  // Offset to pixel below (pixel pitch vertically)
    vec3   u, v, w;        // Camera frame basis vectors
    vec3   defocus_disk_u; // Defocus disk horizontal radius
    vec3   defocus_disk_v; // Defocus disk vertical radius

    void initialize() {
        image_height = static_cast<int>(image_width / aspect_ratio);
        image_height = (image_height < 1) ? 1 : image_height;

        center = lookfrom;

        // Camera - Determine viewport dimensions
        double theta = degree_to_radians(vfov);
        double h = tan(theta/2);
        double viewport_height = 2.0 * h * focus_dist;
        double viewport_width = viewport_height * (static_cast<double>(image_width)/image_height);  // viewport are real, so can be less than one and non-integer

        // Calculate u,v,w basis vectors 
        w = unit_vector(lookfrom - lookat);
        u = unit_vector(cross(vup, w));
        v = cross(w, u);

        // vectors accross the horizontal and down the vertical viewport edges
        vec3 viewport_u = viewport_width * u; 
        vec3 viewport_v = viewport_height * -v;

        // pixel pitch vectors
        pixel_delta_u = viewport_u / image_width;
        pixel_delta_v = viewport_v / image_height;

        // location of the upper left pixel
        vec3 viewport_upper_left = center - (focus_dist * w) - viewport_u/2 - viewport_v/2;
        pixel00_loc = viewport_upper_left + 0.5 * (pixel_delta_u + pixel_delta_v);

        // Camera defocus disk basis vectors
        double defocus_radius = focus_dist * tan(degree_to_radians(defocus_angle / 2));
        defocus_disk_u = u * defocus_radius;
        defocus_disk_v = v * defocus_radius;
    }

    ray get_ray(int i, int j) const {
        // Randomly sampled camera ray for the pixel at location i, j
        point3 pixel_center = pixel00_loc + (i * pixel_delta_u) + (j * pixel_delta_v);
        point3 pixel_sample = pixel_center + pixel_sample_square();

        //vec3 ray_origin = center;
        vec3 ray_origin = (defocus_angle <= 0) ? center : defocus_disk_sample();
        vec3 ray_direction = pixel_sample - ray_origin;
        double ray_time = random_double();

        return ray(ray_origin, ray_direction, ray_time);
    }

    vec3 pixel_sample_square() const {
        // Returns a random point in the square surrouding the pixel at the origin
        double px = -0.5 + random_double();  // random double \in [-0.5, 0.5)
        double py = -0.5 + random_double();  // random double \in [-0.5, 0.5)
        return (px * pixel_delta_u) + (py * pixel_delta_v);
    }

    point3 defocus_disk_sample() const {
        // Random point in the camera defocus disk
        vec3 p = random_in_unit_disk();
        return center + (p[0] * defocus_disk_u) + (p[1] * defocus_disk_v);
    }

    color ray_color(const ray &r, int depth_remaining, const hittable &world) const {
        hit_record rec;

        if (depth_remaining <= 0) {
            return color(0,0,0);  // this ray has been scattered so many times, it can be considered to be black (limit inplave to stop stack blowing up)
        }

        // If the ray hits nothing, return the background color
        if (!world.hit(r, interval(0.001, infinity), rec)) {  // lower bound of 0.001 to ignore second intersections of reflected rays that have been floating point errored to be within the sphere. (Reduces the shadow acne problem)
            return background;
        }

        ray scattered;
        color attenuation;
        color color_from_emission = rec.mat->emitted(rec.u, rec.v, rec.p);

        if (!rec.mat->scatter(r, rec, attenuation, scattered)) {
            return color_from_emission;
        }

        color color_from_scatter = attenuation * ray_color(scattered, depth_remaining-1, world);
        return color_from_emission + color_from_scatter;
    }

};

#endif