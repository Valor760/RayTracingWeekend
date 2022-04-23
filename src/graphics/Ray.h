#ifndef RAY_H
#define RAY_H

#include "vec3.h"

namespace RTWeekend::Graphics {
class Ray {
public:
    Point3 origin;
    vec3 direction;

    Ray() {}
    Ray(const Point3& _orig, const vec3& _dir) : origin(_orig), direction(_dir) {}

    Point3 at(double t) const;
};
}
#endif