#pragma once
#include "vec3.hpp"

class ray {
public:
    ray() {}
    ray(const vec3& _o, const vec3& _d, float _t = 0.0) : o(_o), d(_d), t(_t) {}
    vec3 origin() const { return o; }
    vec3 direction() const { return d; }
    float time() const { return t; }
    vec3 point_at_parameter(float _t) const { return o + _t * d; }

    vec3 o;
    vec3 d;
    float t;
};