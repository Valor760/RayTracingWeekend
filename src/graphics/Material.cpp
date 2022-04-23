#include "Material.h"

namespace RTWeekend::Graphics {
bool Diffuse::scatter(
    const Ray& ray_in, const HitRecord& rec, Color& attenuation, Ray& scattered
) const {
    auto scatter_direction = rec.normal + random_unit_vector();

    if (scatter_direction.near_zero()) {
        scatter_direction = rec.normal;
    }

    scattered = Ray(rec.p, scatter_direction);
    attenuation = albedo;
    return true;
}

bool Metal::scatter(
    const Ray& ray_in, const HitRecord& rec, Color& attenuation, Ray& scattered
) const {
    vec3 reflected = reflect(unit_vector(ray_in.direction), rec.normal);
    scattered = Ray(rec.p, reflected + fuzz * random_in_unit_sphere());
    attenuation = albedo;
    return(dot(scattered.direction, rec.normal) > 0);
}
bool Glass::scatter(
    const Ray& ray_in, const HitRecord& rec, Color& attenuation, Ray& scattered
) const {
    attenuation = Color(1.0, 1.0, 1.0);
    double refraction_ratio = rec.front_face ? (1.0 / ir) : ir;

    vec3 unit_direction = unit_vector(ray_in.direction);

    double cos_theta = fmin(dot(-unit_direction, rec.normal), 1.0);
    double sin_theta = std::sqrt(1.0 - cos_theta * cos_theta);

    bool cannot_refract = refraction_ratio * sin_theta > 1.0;

    vec3 direction;
    if (cannot_refract || reflectance(cos_theta, refraction_ratio) > random_double()) {
        direction = reflect(unit_direction, rec.normal);
    }
    else {
        direction = refract(unit_direction, rec.normal, refraction_ratio);
    }

    scattered = Ray(rec.p, direction);
    return true;
}

double Glass::reflectance(double cosine, double ref_idx) {
    // Schlick's approximation
    auto r0 = (1 - ref_idx) / (1 + ref_idx);
    r0 = r0 * r0;
    return r0 + (1 - r0) * pow((1 - cosine), 5);
}
} // namespace RTWeekend::Graphics