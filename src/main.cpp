#include <iostream>

#include "common.h"
#include "vec3.h"
#include "color.h"
#include "ray.h"
#include "hittable.h"
#include "hittable_list.h"
#include "sphere.h"
#include "camera.h"
#include "material.h"
#include "utils/thread_pool.h"

hittable_list random_scene() {
    hittable_list world;

    auto ground_material = make_shared<diffuse>(color(0.5, 0.5, 0.5));
    world.add(make_shared<sphere>(point3(0,-1000,0), 1000, ground_material));

    for (int a = -11; a < 11; a++) {
        for (int b = -11; b < 11; b++) {
            auto choose_mat = random_double();
            point3 center(a + 0.9*random_double(), 0.2, b + 0.9*random_double());

            if ((center - point3(4, 0.2, 0)).length() > 0.9) {
                shared_ptr<material> sphere_material;

                if (choose_mat < 0.8) {
                    // diffuse
                    auto albedo = color::random() * color::random();
                    sphere_material = make_shared<diffuse>(albedo);
                    world.add(make_shared<sphere>(center, 0.2, sphere_material));
                } else if (choose_mat < 0.95) {
                    // metal
                    auto albedo = color::random(0.5, 1);
                    auto fuzz = random_double(0, 0.5);
                    sphere_material = make_shared<metal>(albedo, fuzz);
                    world.add(make_shared<sphere>(center, 0.2, sphere_material));
                } else {
                    // glass
                    sphere_material = make_shared<glass>(1.5);
                    world.add(make_shared<sphere>(center, 0.2, sphere_material));
                }
            }
        }
    }

    auto material1 = make_shared<glass>(1.5);
    world.add(make_shared<sphere>(point3(0, 1, 0), 1.0, material1));

    auto material2 = make_shared<diffuse>(color(0.4, 0.2, 0.1));
    world.add(make_shared<sphere>(point3(-4, 1, 0), 1.0, material2));

    auto material3 = make_shared<metal>(color(0.7, 0.6, 0.5), 0.0);
    world.add(make_shared<sphere>(point3(4, 1, 0), 1.0, material3));

    return world;
}

color ray_color(const ray& r, const hittable& world, int depth) {
    hit_record rec;

    // Ray bounces
    if(depth <= 0)
        return color(0, 0, 0);

    if(world.hit(r, 0.001, infinity, rec)) {
        ray scattered;
        color attenuation;
        if(rec.mat_ptr->scatter(r, rec, attenuation, scattered))
            return attenuation * ray_color(scattered, world, depth - 1);
        return color(0, 0, 0);
    }

    vec3 unit_direction = unit_vector(r.direction);
    auto t = 0.5 * (unit_direction.y() + 1.0);
    return (1.0 - t) * color(1.0, 1.0, 1.0) + t * color(0.5, 0.7, 1.0);
}

void CalculatePixelColor(
    const int i,
    const int j,
    const int width, 
    const int height, 
    const int samples,
    const int depth,
    const camera& cam,
    const hittable_list& world
    ) {
    color pixel_color(0, 0, 0);
    for(int s = 0; s < samples; s++) {
        auto u = double(i + random_double()) / (width - 1);
        auto v = double(j + random_double()) / (height - 1);

        ray r = cam.get_ray(u, v);

        pixel_color += ray_color(r, world, depth);
    }
    write_color(std::cout, pixel_color, samples);
}

int main(int argc, char** argv) {
    // Image
    constexpr auto aspect_ratio = 16.0 / 9.0;
    constexpr int image_width = 400;
    const int image_height = static_cast<int>(image_width / aspect_ratio);

    constexpr int samples_per_pixel = 10;
    constexpr int max_depth = 5;

    // World
    hittable_list world = random_scene();

    // auto material_ground = make_shared<diffuse>(color(0.8, 0.8, 0.0));
    // auto material_center = make_shared<glass>(1.5);
    // auto material_left   = make_shared<glass>(1.5);
    // auto material_right  = make_shared<metal>(color(0.8, 0.6, 0.2), 1.0);

    // world.add(make_shared<sphere>(point3( 0.0, -100.5, -1.0), 100.0, material_ground));
    // world.add(make_shared<sphere>(point3( 0.0,    0.0, -1.0),   0.5, material_center));
    // world.add(make_shared<sphere>(point3(-1.0,    0.0, -1.0),   0.5, material_left));
    // world.add(make_shared<sphere>(point3( 1.0,    0.0, -1.0),   0.5, material_right));

    // Camera
    point3 lookfrom(13, 2, 3);
    point3 lookat(0, 0, 0);
    camera cam(lookfrom, lookat, vec3(0,1,0), 45, aspect_ratio);

    RTW::Utils::ThreadPool* pool = new RTW::Utils::ThreadPool(1);

    // Render
    std::cout << "P3\n" << image_width << ' ' << image_height << "\n255\n";
    for(int j = image_height-1; j >= 0; --j) {
        std::cerr << "\rLines remaining: " << j << ' ' << std::flush;

        for(int i = 0; i < image_width; ++i) {
            pool->AddTask(CalculatePixelColor,
                i, j, image_width, image_height,
                samples_per_pixel, max_depth,
                cam, world
            );
        }
    }
    std::cerr << "\nDone\n";

    return 0;
}