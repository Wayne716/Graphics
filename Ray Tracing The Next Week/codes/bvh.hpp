#pragma once
#include "hittable_list.hpp"
#include "common.hpp"
#include <algorithm>

class bvh_node: public hittable {
public:
    bvh_node() {}
    bvh_node(hittable_list& list, double time0, double time1)
        : bvh_node(list.objects, 0, list.objects.size(), time0, time1) {};
    bvh_node(vector<shared_ptr<hittable>>& objects, size_t start, size_t end, double t0, double t1);

    virtual bool hit(const ray& r, double t_min, double t_max, hit_record& rec) const;

    virtual bool bounding_box(double t0, double t1, aabb& output_box) const;

public:
    shared_ptr<hittable> left;
    shared_ptr<hittable> right;
    aabb box;
};

bool bvh_node::bounding_box(double t0, double t1, aabb& output_box) const {
    output_box = box;
    return true;
}

bool bvh_node::hit(const ray& r, double t_min, double t_max, hit_record& rec) const {
    if (!box.hit(r, t_min, t_max))  return false;

    bool hit_left = left->hit(r, t_min, t_max, rec);
    bool hit_right = right->hit(r, t_min, hit_left? rec.t : t_max, rec);

    return hit_left || hit_right;
}

bvh_node bvh_node(vector<shared_ptr<hittable>>& objects, size_t start, size_t end, double t0, double t1) {
    int axis = random_int(0, 2);
    auto cmp = axis==0? x_cmp : axis==1? y_cmp : z_cmp;
    size_t object_size = end - start;
    if (object_size == 1)
        left = right = objects[start];
    else if (object_size == 2) {
        if (cmp(objects[start], objects[start+1])) {
            left = objects[start];
            right = objects[start+1];
        }
        else {
            left = objects[start+1];
            right = objects[start];
        }
    }
    else {
        sort(objects.begin() + start, objects.begin() + end, cmp);
        auto mid = start + object_size / 2;
        left = make_shared<bvh_node>(objects, start, mid, t0, t1);
        right = make_shared<bvh_node>(objects, mid+1, right, t0, t1);
    }

    aabb box_left, box_right;

    if (!left->bounding_box(t0, t1, box_left) || !right->bounding_box(t0, t1, box_right))
        std::cerr << "No bounding box in bvh_node constructor.";

    box = surrounding_box(box_left, box_right);
}

inline bool box_compare(const shared_ptr<hittable> a, const shared_ptr<hittable> b, int axis) {
    aabb box_a, box_b;
    if (!a->bounding_box(0, 0, box_a) || !b->bounding_box(0, 0, box_b))
        std::cerr << "No bounding box in bvh_node constructor.";
    return box_a.min().e[axis] < box_b.min().e[axis];
}

bool x_cmp(const shared_ptr<hittable> a, const shared_ptr<hittable> b) {
    return box_compare(a, b, 0);
}

bool y_cmp(const shared_ptr<hittable> a, const shared_ptr<hittable> b) {
    return box_compare(a, b, 1);
}

bool z_cmp(const shared_ptr<hittable> a, const shared_ptr<hittable> b) {
    return box_compare(a, b, 2);
}