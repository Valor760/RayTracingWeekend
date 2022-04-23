#include "HittableList.h"

namespace RTWeekend::Graphics {
 HittableList::HittableList(shared_ptr<Hittable> obj) { add(obj); }

void HittableList::add(shared_ptr<Hittable> obj) { objects.push_back(obj); }
void HittableList::clear() { objects.clear(); }

bool HittableList::hit(const Ray& r, double t_min, double t_max, HitRecord& rec) const {
    HitRecord temporary_rec;
    bool hit_anything = false;
    auto closest_so_far = t_max;

    for (const auto& obj : objects) {
        if (obj->hit(r, t_min, closest_so_far, temporary_rec)) {
            hit_anything = true;
            closest_so_far = temporary_rec.t;
            rec = temporary_rec;
        }
    }

    return hit_anything;
}
} // namespace RTWeekend::Graphics