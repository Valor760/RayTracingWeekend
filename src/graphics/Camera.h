#ifndef CAMERA_H
#define CAMERA_H

#include "Ray.h"
//#include "common.h"

namespace RTWeekend::Graphics {
class Camera {
public:
    Camera(
        Point3 lookfrom,
        Point3 lookat,
        vec3 vup,
        double v_fov,
        double aspect_ratio
    );
    Ray GetRay(double u, double v) const;

private:
    Point3 origin;
    Point3 lower_left_corner;
    vec3 horizontal;
    vec3 vertical;
};
} // namespace RTWeekend::Graphics
#endif