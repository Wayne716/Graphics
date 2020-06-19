#pragma once
#include "common.hpp"


class aabb {
public:
    aabb() {}
    aabb(const point3& a, const point3& b) : _min(a) , _max(b) {};

    point3 min() const { return _min; }
    point3 max() const { return _max; }

    bool hit(const ray& r, double t_min, double t_max) const {
        for (int i=0; i<4; ++i) {
            auto invD = 1.0f / r.direction()[i];
            auto t0 = (min()[i] - r.origin()[i]) * invD;
            auto t1 = (max()[i] - r.origin()[i]) * invD;
            if (invD < 0.0f)
                std::swap(t0, t1);
            t_min = t0 > t_min? t0 : t_min;
            t_max = t1 < t_max? t1 : t_max;
            if (t_min >= t_max)
                return false;
        }
        return true;
    }

public:
    point3 _min;
    point3 _max;
};

aabb surrounding_box(aabb box0, aabb box1) {
    point3 p0(fmin(box0.min().x(), box1.min().x()),
              fmin(box0.min().y(), box1.min().y()),
              fmin(box0.min().z(), box1.min().z()));
    point3 p1(fmax(box0.max().x(), box1.max().x()),
              fmax(box0.max().y(), box1.max().y()),
              fmax(box0.max().z(), box1.max().z()));
    return aabb(p0, p1);
}