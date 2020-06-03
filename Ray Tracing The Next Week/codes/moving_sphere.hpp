#pragma once
#include "sphere.hpp"

class moving_sphere: public hitable {
public:
    moving_sphere() {}
    moving_sphere(vec3 cen0, vec3 cen1, float t0, float t1, float r, material *m) :
            center0(cen0), center1(cen1), time0(t0), time1(t1), radius(r), mat(m) {};
    virtual bool hit(const ray& r, float tmin, float tmax, hit_record& rec) const;
    virtual bool boundingBox(float t0, float t1, aabb &box) const;
    vec3 center(float time) const;
    vec3 center0, center1;
    float time0, time1;
    float radius;
    material* mat;
};

bool moving_sphere::boundingBox(float t0, float t1, aabb &box) const {
    aabb box1 = aabb(center(t0) - vec3(radius, radius, radius), center(t0) + vec3(radius, radius, radius));
    aabb box2 = aabb(center(t1) - vec3(radius, radius, radius), center(t1) + vec3(radius, radius, radius));
    box = surroundingBox(box1, box2);
    return true;
}


vec3 moving_sphere::center(float time) const {
    return center0 + (time - time0) / (time1 - time0) * (center1 - center0);
}

bool moving_sphere::hit(const ray &r, float tmin, float tmax, hit_record &rec) const {
    vec3 oc = r.origin() - center(r.time());
    float a = dot(r.direction(), r.direction());
    float b = dot(oc, r.direction());
    float c = dot(oc, oc) - radius * radius;
    float discriminant = b * b - a * c;
    if (discriminant > 0) {
        float temp = (-b - sqrt(discriminant)) / a;
        if (temp > tmin && temp < tmax) {
            rec.t = temp;
            rec.p = r.point_at_parameter(rec.t);
            rec.normal = (rec.p - center(r.time())) / radius;
            rec.mat = mat;
            return true;
        }
        temp = (-b + sqrt(discriminant)) / a;
        if (temp > tmin && temp < tmax) {
            rec.t = temp;
            rec.p = r.point_at_parameter(rec.t);
            rec.normal = (rec.p - center(r.time())) / radius;
            rec.mat = mat;
            return true;
        }
    }
    return false;
}