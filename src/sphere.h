#ifndef SPHERE_H
#define SPHERE_H

#include "hittable.h"


class sphere : public hittable {
    public:
        point3 center;
        double radius;


        sphere() {}
        sphere(point3 _center, double _radius) : center(_center), radius(_radius) {}

        virtual bool hit(const ray& r, double t_min, double t_max, hit_record& rec) const override;
};

bool sphere::hit(const ray& r, double t_min, double t_max, hit_record& rec) const {
    vec3 oc = r.origin - center;
    auto a = r.direction.length_squared();
    auto half_b = dot(oc, r.direction);
    auto c = oc.length_squared() - radius * radius;

    auto discriminant = half_b*half_b - a*c;
    if(discriminant < 0) return false;

    auto sqrtd = std::sqrt(discriminant); // Because we use it 2 times. Expensive operation
    auto root = (-half_b - sqrtd) / a;
    if(root < t_min || root > t_max) {
        root = (-half_b + sqrtd) / a;
        if(root < t_min || root > t_max)
            return false; // If we didnt hit with any of the root
    }

    rec.t = root;
    rec.p = r.at(rec.t);
    vec3 outward_normal = (rec.p - center) / radius;
    rec.set_face_normal(r, outward_normal);

    return true;
}


#endif