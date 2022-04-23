#include "Camera.h"

namespace RTWeekend::Graphics {
Camera::Camera(
    Point3 lookfrom,
    Point3 lookat,
    vec3 vup,
    double v_fov,
    double aspect_ratio
) {
    // Camera
    auto theta = deg_to_rad(v_fov);
    auto h = tan(theta / 2);
    float viewport_height = 2.0 * h;
    float viewport_width = aspect_ratio * viewport_height;

    auto w = unit_vector(lookfrom - lookat);
    auto u = unit_vector(cross(vup, w));
    auto v = cross(w, u);

    origin = lookfrom;
    horizontal = viewport_width * u;
    vertical = viewport_height * v;
    lower_left_corner = origin - horizontal / 2 - vertical / 2 - w;
}

Ray Camera::GetRay(double u, double v) const {
    return Ray(origin, lower_left_corner + u * horizontal + v * vertical - origin);
}
} // namespace RTWeekend::Graphics