#pragma once
#include "texture.hpp"

class material {
public:
    virtual bool scatter(const ray& r_in, const hit_record& rec,
            color& attenuation, ray& scattered) const = 0;
    virtual color emitted(double u, double v, const point3& p) const {
        return color(0, 0, 0);
    }
};

class lambertian : public material {
public:
    lambertian(shared_ptr<texture> a) : albedo(a) {}

    virtual bool scatter(const ray& r_in, const hit_record& rec,
            color& attenuation, ray& scatterd) const {
        // hack method not lambertian
        vec3 scatter_direction = rec.normal + random_unit_vector();
        scatterd = ray(rec.p, scatter_direction, r_in.time());
        attenuation = albedo->value(rec.u, rec.v, rec.p);
        return true;
    }

public:
    shared_ptr<texture> albedo;
};

class metal : public material {
public:
    metal(const color& a, double f) : albedo(a), fuzz(f < 1? f : 1) {};

    virtual bool scatter(const ray& r_in, const hit_record& rec,
            color& attenuation, ray& scattered) const {
        vec3 reflected = reflect(unit_vector(r_in.direction()), rec.normal);
        scattered = ray(rec.p, reflected + fuzz * random_in_unit_sphere());
        attenuation = albedo;
        return dot(scattered.direction(), rec.normal) > 0;
    }

public:
    color albedo;
    double fuzz;
};

double schlick(double cosine, double ref_index) {
    auto r0 = (1 - ref_index) / (1 + ref_index);
    r0 *= r0;
    return r0 + (1 - r0) * pow((1 - cosine), 5);
}


class dielectric : public material {
public:
    dielectric(double ri) : ref_index(ri) {};
    virtual bool scatter(const ray& r_in, const hit_record& rec,
            color& attenuation, ray& scattered) const {
        attenuation = color(1.0, 1.0, 1.0);
        double in_over_out = rec.front_face? 1 / ref_index : ref_index;
        vec3 unit_direction = unit_vector(r_in.direction());

        double cos_theta = fmin(dot(-unit_direction, rec.normal), 1.0);
        double sin_theta = sqrt(1 - cos_theta * cos_theta);

        if (in_over_out * sin_theta > 1.0) {
            vec3 reflected = reflect(unit_direction, rec.normal);
            scattered = ray(rec.p, reflected);
            return true;
        }

        double reflect_prob = schlick(cos_theta, in_over_out);
        if (random_double() < reflect_prob) {
            vec3 reflected = reflect(unit_direction, rec.normal);
            scattered = ray(rec.p, reflected);
            return true;
        }
        vec3 refracted = refract(unit_direction, rec.normal, in_over_out);
        scattered = ray(rec.p, refracted);
        return true;
    }

    double ref_index;
};


class area_light : public material {
public:
    area_light(shared_ptr<texture> a) : emit(a) {}

    virtual bool scatter(const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered) const {
        return false;
    }

    virtual color emitted(double u, double v, const point3& p) const {
        return emit->value(u, v, p);
    }

public:
    shared_ptr<texture> emit;
};


class isotrophic : public material {
public:
    isotrophic(shared_ptr<texture> a) : albedo(a) {};

    virtual bool scatter(const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered) const {
        scattered = ray(rec.p, random_in_unit_sphere(), r_in.time());
        attenuation = albedo->value(rec.u, rec.v, rec.p);
        return true;
    }

public:
    shared_ptr<texture> albedo;
};