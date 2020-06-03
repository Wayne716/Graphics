#pragma once
#include "perlin.hpp"

class texture {
public:
    virtual vec3 value(float u, float v, const vec3& p) const = 0;
};

class constant_texture : public texture {
public:
    constant_texture() {}
    constant_texture(vec3 c) : color(c) {};

    virtual vec3 value(float u, float v, const vec3& p) const {
        return color;
    }

    vec3 color;
};

class checker_texture : public texture {
public:
    checker_texture() {}
    checker_texture(texture *t0, texture *t1): even(t0), odd(t1) {};

    virtual vec3 value(float u, float v, const vec3 &p) const {
        float sine = sin(5*p.x()) * sin(5*p.y()) * sin(5*p.z());
        if (sine < 0)
            return odd->value(u, v, p);
        else
            return even->value(u, v, p);
    }

    texture *odd;
    texture *even;
};

class noise_texture : public texture {
public:
    noise_texture() {}
    noise_texture(float _scale) : scale(_scale) {};

    virtual vec3 value(float u, float v, const vec3 &p) const {
        return vec3(1,1,1) * 0.5 * (1 + sin(scale * p.z() + 10 * noise.turbulence(p)));
    }

    float scale;
    perlin noise;
};