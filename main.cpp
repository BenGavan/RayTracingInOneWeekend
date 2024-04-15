#include "color.h"
#include "vec3.h"

#include <iostream>

int main() {
    //std::cout << "Ray tracing in One Weekend" << std::endl;

    float aspect_ratio = 16.0 / 9.0;
    int image_width = 256;
    int image_height = static_cast<int>(image_width / aspect_ratio);
    image_height = (image_height < 1) ? 1 : image_height;

    float viewport_height = 2.0;
    int viewport_width = viewport_height * (static_cast<double>(image_width)/image_height);  // viewport are real, so can be less than one and non-integer

    std::cout << "P3\n";  // P3 := colors are in ASCII
    std::cout << image_width << ' ' << image_height << '\n'; // Image width & height (i.e. # columns and rows)
    std::cout << "255\n"; // 255 := Max color

    for (int j=0; j < image_height; ++j) {  // write pixels out in rows
        std::clog << "\rScanlines remaining: " << (image_height - j) << ' ' << std::flush;
        for (int i=0; i<image_width; ++i) {  // each row is written out left to right

            // each rbg component is represented internally by x \in [0, 1] 
            //  so must be scaled to ints between 0 & 255
            color pixel_color = color(double(i)/(image_width-1), double(j)/(image_height-1), 0);
            write_color(std::cout, pixel_color);
        }
    }

    std::clog << "\rDone :)                \n";

    return 0;
}