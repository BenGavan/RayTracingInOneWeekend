#ifndef RTWEEKEND_H
#define RTWEEKEND_H

#include <cmath>
#include <limits>
#include <memory>

// usings
using std::shared_ptr;
using std::make_shared;
using std::sqrt;

//consts
const double infinity = std::numeric_limits<double>::infinity();
const double pi = 3.1415926535897931159979634685;

// utility funcs
inline double degree_to_radians(double deg) {
    return deg * pi / 180.0;
}

// common headers
#include "interval.h"
#include "ray.h"
#include "vec3.h"

#endif