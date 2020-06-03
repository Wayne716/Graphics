#pragma once
#include "hitable.hpp"

class sphere: public hitable {
public:
    sphere() {}
    sphere(vec3 _center, float _radius, material* _mat): center(_center), radius(_radius), mat(_mat) {}
    virtual bool hit(const ray& r, float t_min, float t_max, hit_record& rec) const;
    virtual bool boundingBox(float t0, float t1, aabb &box) const;
    vec3 center;
    float radius;
    material *mat;
};

bool sphere::boundingBox(float t0, float t1, aabb &box) const {
    box = aabb(center - vec3(radius, radius, radius), center + vec3(radius, radius, radius));
    return true;
}

bool sphere::hit(const ray &r, float t_min, float t_max, hit_record &rec) const {
    vec3 oc = r.origin() - center;
    float a = dot(r.direction(), r.direction());
    float b = dot(oc, r.direction());
    float c = dot(oc, oc) - radius*radius;
    float discriminant = b * b - a * c;
    if (discriminant > 0) {
        float temp = (-b - sqrt(discriminant))/a;
        if (t_min < temp && temp < t_max) {
            rec.t = temp;
            rec.p = r.point_at_parameter(rec.t);
            rec.normal = (rec.p - center) / radius;
            rec.mat = this->mat;
            return true;
        }
        temp = (-b + sqrt(discriminant))/a;
        if (t_min < temp && temp < t_max) {
            rec.t = temp;
            rec.p = r.point_at_parameter(rec.t);
            rec.normal = (rec.p - center) / radius;
            rec.mat = this->mat;
            return true;
        }
    }
    return false;
}