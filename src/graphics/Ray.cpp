#include "Ray.h"

namespace RTWeekend::Graphics {
Point3 Ray::at(double t) const {
    return origin + t * direction;
}
} // namespace RTWeekend::Graphics