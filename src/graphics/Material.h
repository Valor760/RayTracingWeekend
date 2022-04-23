#ifndef MATERIAL_H
#define MATERIAL_H

#include "Ray.h"
#include "Hittable.h"

namespace RTWeekend::Graphics {
struct HitRecord;

class Material {
public:
    virtual bool scatter(
        const Ray& ray_in, const HitRecord& rec, Color& attenuation, Ray& scattered
    ) const = 0;
};

class Diffuse : public Material {
public:
    Color albedo;


    Diffuse(const Color& a) : albedo(a) {}

    virtual bool scatter(
        const Ray& ray_in, const HitRecord& rec, Color& attenuation, Ray& scattered
    ) const override;
};

class Metal : public Material {
public:
    Color albedo;
    double fuzz;


    Metal(const Color& a, double f) : albedo(a), fuzz(f < 1 ? f : 1) {}
    virtual bool scatter(
        const Ray& ray_in, const HitRecord& rec, Color& attenuation, Ray& scattered
    ) const override;
};

class Glass : public Material {
public:
    double ir; // Index of refraction


    Glass(double ind_of_refr) : ir(ind_of_refr) {}

    virtual bool scatter(
        const Ray& ray_in, const HitRecord& rec, Color& attenuation, Ray& scattered
    ) const override;

private:
    static double reflectance(double cosine, double ref_idx);
};
}
#endif