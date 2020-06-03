#pragma once
#include "aabb.hpp"

class material;

struct hit_record {
    float t;
    vec3 p; // exact hit point
    vec3 normal;
    material *mat;
};

class hitable {
public:
    virtual bool hit(const ray& r, float t_min, float t_max, hit_record& rec) const = 0;
    virtual bool boundingBox(float t0, float t1, aabb& box) const = 0;
};