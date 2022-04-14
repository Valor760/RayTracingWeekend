#ifndef COLOR_H
#define COLOR_H

#include <iostream>
#include <vector>

#include "vec3.h"
#include "common.h"


void write_color(color pixel_color, int samples_per_pixel, std::vector<Pixel>* buffer) {
    auto r = pixel_color.x();
    auto g = pixel_color.y();
    auto b = pixel_color.z();

    // Divide the color by the number of samples and gamma correct
    auto scale = 1.0 / samples_per_pixel;
    r = std::sqrt(scale * r);
    g = std::sqrt(scale * g);
    b = std::sqrt(scale * b);

    Pixel p = {static_cast<int>(256 * clamp(r, 0.0, 0.999)),
               static_cast<int>(256 * clamp(g, 0.0, 0.999)),
               static_cast<int>(256 * clamp(b, 0.0, 0.999))};
    
    buffer->push_back(p);
}


#endif