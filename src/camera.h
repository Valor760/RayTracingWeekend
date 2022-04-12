#ifndef CAMERA_H
#define CAMERA_H

#include "vec3.h"
#include "ray.h"
#include "common.h"

class camera {
    public:
        camera(
            point3 lookfrom,
            point3 lookat,
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

        ray get_ray(double u, double v) const {
            return ray(origin, lower_left_corner + u * horizontal + v * vertical - origin);
        }


    private:
        point3 origin;
        point3 lower_left_corner;
        vec3 horizontal;
        vec3 vertical;
};
#endif