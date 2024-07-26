#ifndef RTW_STB_IMAGE_H
#define RTW_STB_IMAGE_H


// disable strict warning from the Microsoft Visual C++ compiler
#ifdef _MSC_VER
    #pragma warning (push, 0)
#endif


#define STB_IMAGE_IMPLEMENTATION
#define STBI_FAILURE_USERMSG

#include "external/stb_image.h"

#include <cstdlib>
#include <iostream>

class rtw_image {
public:
    rtw_image() : data(nullptr) {}

    rtw_image(const char* image_filename) {
        std::string filename = std::string(image_filename);
        auto imagedir = getenv("RTM_IMAGES");

        if (imagedir && load(std::string(imagedir) + "/" + image_filename)) return;
        if (load(filename)) return;
        if (load("images/" + filename)) return;
        if (load("../images/" + filename)) return;
        if (load("../../images/" + filename)) return;
        if (load("../../../images/" + filename)) return;
        if (load("../../../../images/" + filename)) return;
        if (load("../../../../../images/" + filename)) return;
        if (load("../../../../../../images/" + filename)) return;

        std::cerr << "ERROR: Could not load image file '" << image_filename << "'.\n";
    }

    ~rtw_image() { STBI_FREE(data); }

    bool load(const std::string filename) {
        int n = bytes_per_pixel;
        data = stbi_load(filename.c_str(), &image_width, &image_height, &n, bytes_per_pixel);
        bytes_per_scanline = image_width * bytes_per_pixel;
        return data != nullptr;
    }

    int width() const { return (data == nullptr) ? 0 : image_width; }
    int height() const { return (data == nullptr) ? 0 : image_height; }

    const unsigned char* pixel_data(int x, int y) const {
        static unsigned char magenta[] = { 255, 0, 255 };
        if (data == nullptr) return magenta;

        x = clamp(x, 0, image_width);
        y = clamp(y, 0, image_height);

        return data + y*bytes_per_scanline + x*bytes_per_pixel;
    }

private:
    const int bytes_per_pixel = 3;
    unsigned char *data;
    int image_width, image_height;
    int bytes_per_scanline;

    static int clamp(int x, int low, int high) {
        // clamp x \in [low, high)
        if (x < low) return low;
        if (x < high) return x;
        return high-1;
    }
};

// restore MSVC compiler warnings
#ifdef _MSC_VER
    #pragma warning (pop)
#endif


#endif