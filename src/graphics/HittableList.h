#ifndef HITTABLE_LIST_H
#define HITTABLE_LIST_H

#include <vector>
#include <memory>

#include "Hittable.h"


using std::shared_ptr;
using std::make_shared;

namespace RTWeekend::Graphics {
class HittableList : public Hittable {
public:
    std::vector<shared_ptr<Hittable>> objects;


    HittableList() {}
    HittableList(shared_ptr<Hittable> obj);

    void add(shared_ptr<Hittable> obj);
    void clear();
    virtual bool hit(const Ray& r, double t_min, double t_max, HitRecord& rec) const override;
};
} // namespace RTWeekend::Graphics
#endif