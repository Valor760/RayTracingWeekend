#include "Renderer.h"

#include <vector>
#include <future>
#include <thread>

#include "utils/thread_pool.h"
#include "graphics/Material.h"
#include "graphics/Sphere.h"

namespace RTWeekend {
void Renderer::StartRender() {
    auto rendering_thread = std::thread{ &Renderer::Render, this, m_buffer };
}

void Renderer::Render(unsigned char* buffer) {
    Graphics::HittableList world = randomScene();

    // Initialize camera
    Graphics::Point3 lookfrom(13, 2, 3);
    Graphics::Point3 lookat(0, 0, 0);
    m_camera = std::make_unique<Graphics::Camera>(lookfrom, lookat, Graphics::vec3(0, 1, 0), 25, getAspectRatio());
    
    Utils::ThreadPool pool(7);
    std::vector<std::future<void>> futures;
    futures.reserve(m_width * m_height);

    auto renderPixel = [buffer, this, world](const int i,const int j) 
    {
        Graphics::Color pixel_color(0, 0, 0);
        for (int s = 0; s < m_samples; s++) {
            auto u = double(i + Graphics::random_double()) / (m_width - 1);
            auto v = double(j + Graphics::random_double()) / (m_height - 1);

            Graphics::Ray r = m_camera->GetRay(u, v);

            pixel_color += rayColor(r, world, m_depth);
        }
        //buffer[j * width + i] = write_color(pixel_color, samples);
        WriteToBuffer(buffer, i, j, pixel_color);
    };

    for (int j = m_height - 1; j >= 0; --j) {
        std::cerr << "\rLines remaining: " << j << ' ' << std::flush;
        for (int i = 0; i < m_width; ++i) {
            futures.emplace_back(
                pool.AddTask(renderPixel, i, j));
        }
    }
    // Wait all tasks
    std::for_each(begin(futures), end(futures), [](auto& future) { future.wait(); });
    std::cerr << "\nDone rendering\n";
}

Graphics::HittableList Renderer::randomScene() {
    Graphics::HittableList world;

    auto ground_material = make_shared<Graphics::Diffuse>(Graphics::Color(0.5, 0.5, 0.5));
    world.add(make_shared<Graphics::Sphere>(Graphics::Point3(0, -1000, 0), 1000, ground_material));

    for (int a = -11; a < 11; a++) {
        for (int b = -11; b < 11; b++) {
            auto choose_mat = Graphics::random_double();
            Graphics::Point3 center(a + 0.9 * Graphics::random_double(), 0.2, b + 0.9 * Graphics::random_double());

            if ((center - Graphics::Point3(4, 0.2, 0)).length() > 0.9) {
                shared_ptr<Graphics::Material> sphere_material;

                if (choose_mat < 0.8) {
                    // diffuse
                    auto albedo = Graphics::Color::random() * Graphics::Color::random();
                    sphere_material = make_shared<Graphics::Diffuse>(albedo);
                    world.add(make_shared<Graphics::Sphere>(center, 0.2, sphere_material));
                }
                else if (choose_mat < 0.95) {
                    // metal
                    auto albedo = Graphics::Color::random(0.5, 1);
                    auto fuzz = Graphics::random_double(0, 0.5);
                    sphere_material = make_shared<Graphics::Metal>(albedo, fuzz);
                    world.add(make_shared<Graphics::Sphere>(center, 0.2, sphere_material));
                }
                else {
                    // glass
                    sphere_material = make_shared<Graphics::Glass>(1.5);
                    world.add(make_shared<Graphics::Sphere>(center, 0.2, sphere_material));
                }
            }
        }
    }

    auto material1 = make_shared<Graphics::Glass>(1.5);
    world.add(make_shared<Graphics::Sphere>(Graphics::Point3(0, 1, 0), 1.0, material1));

    auto material2 = make_shared<Graphics::Diffuse>(Graphics::Color(0.4, 0.2, 0.1));
    world.add(make_shared<Graphics::Sphere>(Graphics::Point3(-4, 1, 0), 1.0, material2));

    auto material3 = make_shared<Graphics::Metal>(Graphics::Color(0.7, 0.6, 0.5), 0.0);
    world.add(make_shared<Graphics::Sphere>(Graphics::Point3(4, 1, 0), 1.0, material3));

    return world;
}

Graphics::Color Renderer::rayColor(const Graphics::Ray& r, const Graphics::Hittable& world, int depth) {
    Graphics::HitRecord rec;

    // Ray bounces
    if (depth <= 0)
        return Graphics::Color(0, 0, 0);

    if (world.hit(r, 0.001, Graphics::infinity, rec)) {
        Graphics::Ray scattered;
        Graphics::Color attenuation;
        if (rec.mat_ptr->scatter(r, rec, attenuation, scattered))
            return attenuation * rayColor(scattered, world, depth - 1);
        return Graphics::Color(0, 0, 0);
    }

    Graphics::vec3 unit_direction = unit_vector(r.direction);
    auto t = 0.5 * (unit_direction.y() + 1.0);
    return (1.0 - t) * Graphics::Color(1.0, 1.0, 1.0) + t * Graphics::Color(0.5, 0.7, 1.0);
}

double Renderer::getAspectRatio() {
    return double(m_width) / double(m_height);
}

void Renderer::WriteToBuffer(unsigned char* buffer, int ix, int iy, Graphics::Color pixel_color) {
    auto r = pixel_color.x();
    auto g = pixel_color.y();
    auto b = pixel_color.z();

    // Divide the color by the number of samples and gamma correct
    auto scale = 1.0 / m_samples;
    r = std::sqrt(scale * r);
    g = std::sqrt(scale * g);
    b = std::sqrt(scale * b);

    // RGB image pixel index
    const unsigned int index = 3 * (iy * m_width + ix);

    buffer[index] = static_cast<unsigned char>(256 * Graphics::clamp(r, 0.0, 0.999));
    buffer[index + 1] = static_cast<unsigned char>(256 * Graphics::clamp(g, 0.0, 0.999));
    buffer[index + 2] = static_cast<unsigned char>(256 * Graphics::clamp(b, 0.0, 0.999));
}
} // namespace RTWeekend