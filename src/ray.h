#ifndef RAY_H
#define RAY_H

#include "vec3.h"


class ray {
    public:
        point3 origin;
        vec3 direction;

        ray() {}
        ray(const point3& _orig, const vec3& _dir) : origin(_orig), direction(_dir) {}

        point3 at(double t) const {
            return origin + t * direction;
        }
};




#endif