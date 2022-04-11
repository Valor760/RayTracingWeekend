#ifndef COMMON_H
#define COMMON_H

#include <cmath>
#include <memory>
#include <limits>
#include <cstdlib>


// Usings
using std::shared_ptr;
using std::make_shared;


// Constants
constexpr double infinity = std::numeric_limits<double>::infinity();
constexpr double pi = 3.1415926535897932385;


// Utility functions
inline double deg_to_rad(const double& deg) {
    return deg * pi / 180.0;
}

inline double random_double() {
    return rand() / (RAND_MAX + 1.0);
}

inline double random_double(double min, double max) {
    return min + (max - min) * random_double();
}

inline double clamp(double x, double min, double max) {
    if(x < min) return min;
    if(x > max) return max;
    return x;
}

// Common headers
#include "ray.h"
#include "vec3.h"

#endif