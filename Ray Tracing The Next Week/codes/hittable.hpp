#pragma once
#include "ray.hpp"
#include "aabb.hpp"

class material;

struct hit_record {
    point3 p;
    vec3 normal;
    bool front_face;
    double t;
    double u, v;
    shared_ptr<material> mat;

    inline void set_face_normal(const ray& r, const vec3& outward_normal) {
        // face to face = dot product < 0
        front_face = dot(r.direction(), outward_normal) < 0;
        normal = front_face? outward_normal : -outward_normal;
    }
};


class hittable {
public:
    virtual bool hit(const ray& r, double t_min, double t_max, hit_record& rec) const = 0;

    // t for moving objects
    virtual bool bounding_box(double t0, double t1, aabb& output_box) const = 0;
};

class flip_face : public hittable {
public:
    flip_face(shared_ptr<hittable> p) : ptr(p) {};

    virtual bool hit(const ray& r, double t_min, double t_max, hit_record& rec) const {
        if (!ptr->hit(r, t_min, t_max, rec))
            return false;
        rec.front_face = !rec.front_face;
        return true;
    }

    virtual bool bounding_box(double t0, double t1, aabb& output_box) const {
        return ptr->bounding_box(t0, t1, output_box);
    }

public:
    shared_ptr<hittable> ptr;
};


class translate : public hittable {
public:
    translate(shared_ptr<hittable> p, const vec3& displacement) : ptr(p), offset(displacement) {};

    virtual bool hit(const ray& r, double t_min, double t_max, hit_record& rec) const;
    virtual bool bounding_box(double t0, double t1, aabb& output_box) const;

public:
    shared_ptr<hittable> ptr;
    vec3 offset;
};

bool translate::hit(const ray& r, double t_min, double t_max, hit_record& rec) const {
    ray moved_r(r.origin() - offset, r.direction(), r.time());
    if (!ptr->hit(moved_r, t_min, t_max, rec))
        return false;
    rec.p += offset;
    rec.set_face_normal(moved_r, rec.normal);
    return true;
}

bool translate::bounding_box(double t0, double t1, aabb& output_box) const {
    if (!ptr->bounding_box(t0, t1, output_box))
        return false;

    output_box = aabb(output_box.min() + offset, output_box.max() + offset);
    return true;
}


class rotate_y : public hittable {
public:
    rotate_y(shared_ptr<hittable> p, double angle);

    virtual bool hit(const ray& r, double t_min, double t_max, hit_record& rec) const;
    virtual bool bounding_box(double t0, double t1, aabb& output_box) const {
        output_box = box;
        return hasbox;
    }

public:
    shared_ptr<hittable> ptr;
    double sin_theta;
    double cos_theta;
    bool hasbox;
    aabb box;
};

rotate_y::rotate_y(shared_ptr<hittable> p, double angle) : ptr(p) {
    auto radian = degree_to_radian(angle);
    sin_theta = sin(radian);
    cos_theta = cos(radian);
    hasbox = ptr->bounding_box(0, 1, box);

    point3 min(infinity, infinity, infinity);
    point3 max(-infinity, -infinity, -infinity);

    for (int i=0; i<2; ++i)
        for (int j=0; j<2; ++j)
            for (int k=0; k<2; ++k) {
                auto x = i * box.max().x() + (1 - i) * box.min().x();
                auto y = j * box.max().y() + (1 - j) * box.min().y();
                auto z = k * box.max().z() + (1 - k) * box.min().z();

                auto newx = cos_theta * x + sin_theta * z;
                auto newz = -sin_theta * x + cos_theta * z;

                vec3 tester(newx, y, newz);

                for (int c=0; c<3; ++c) {
                    min[c] = fmin(min[c], tester[c]);
                    max[c] = fmax(max[c], tester[c]);
                }
            }

    box = aabb(min, max);
}

bool rotate_y::hit(const ray& r, double t_min, double t_max, hit_record& rec) const {
    auto origin = r.origin();
    auto direction = r.direction();

    origin[0] = cos_theta * r.origin()[0] - sin_theta * r.origin()[2];
    origin[2] = sin_theta * r.origin()[0] + cos_theta * r.origin()[2];

    direction[0] = cos_theta * r.direction()[0] - sin_theta * r.direction()[2];
    direction[2] = sin_theta * r.direction()[0] + cos_theta * r.direction()[2];

    ray rotated_r(origin, direction, r.time());

    if (!ptr->hit(rotated_r, t_min, t_max, rec))
        return false;

    auto p = rec.p;
    auto normal = rec.normal;

    // update hit point
    p[0] = cos_theta * rec.p[0] + sin_theta * rec.p[2];
    p[2] = -sin_theta * rec.p[0] + cos_theta * rec.p[2];

    // update intersection normal
    normal[0] = cos_theta * rec.normal[0] + sin_theta * rec.normal[2];
    normal[2] = -sin_theta * rec.normal[0] + sin_theta * rec.normal[2];

    rec.p = p;
    rec.set_face_normal(rotated_r, normal);

    return true;
}