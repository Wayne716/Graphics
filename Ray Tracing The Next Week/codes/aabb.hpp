#pragma once
#include "ray.hpp"

class aabb {
public:
    aabb() {}
    aabb(const vec3 &a, const vec3 &b) : _min(a), _max(b) {};

    vec3 min() const { return _min; }
    vec3 max() const { return _max; }

    bool hit(const ray& r, float tmin, float tmax) const {
        for (int axis = 0; axis < 3; ++axis) {
            float D = 1 / r.direction()[axis];
            float t0 = (min()[axis] - r.origin()[axis]) * D;
            float t1 = (max()[axis] - r.origin()[axis]) * D;
            if (D < 0.0f)
                std::swap(t0, t1);
            tmin = t0 > tmin? t0 : tmin;
            tmax = t1 < tmax? t1 : tmax;
            if (tmin >= tmax)
                return false;
        }
        return true;
    }

    vec3 _min, _max;
};

aabb surroundingBox(aabb box0, aabb box1) {
    vec3 bigMin(fmin(box0.min().x(), box1.min().x()), fmin(box0.min().y(), box1.min().y()), fmin(box0.min().z(), box1.min().z()));
    vec3 bigMax(fmax(box0.max().x(), box1.max().x()), fmax(box0.max().y(), box1.max().y()), fmax(box0.max().z(), box1.max().z()));
    return aabb(bigMin, bigMax);
}