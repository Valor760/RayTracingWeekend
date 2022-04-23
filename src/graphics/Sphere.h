#ifndef SPHERE_H
#define SPHERE_H

#include "Hittable.h"

namespace RTWeekend::Graphics {
class Sphere : public Hittable {
public:
    Point3 center;
    double radius;
    shared_ptr<Material> mat_ptr;

public:
    Sphere() {}
    Sphere(Point3 _center, double _radius, shared_ptr<Material> mat)
        : center(_center), radius(_radius), mat_ptr(mat) {}

    virtual bool hit(const Ray& r, double t_min, double t_max, HitRecord& rec) const override;
};
}

#endif