#pragma once
#include "hitable.hpp"

class bvh_node: hitable {
public:
    bvh_node() {}
    bvh_node(hitable **l, int n, float time0, float time1);
    virtual bool hit(const ray &r, float t_min, float t_max, hit_record &rec) const;
    virtual bool boundingBox(float t0, float t1, aabb &box) const;
    hitable *left;
    hitable *right;
    aabb box;
};

bool bvh_node::boundingBox(float t0, float t1, aabb &ab) const {
    ab = box;
    return true;
}

bool bvh_node::hit(const ray &r, float t_min, float t_max, hit_record &rec) const {
    if (box.hit(r, t_min, t_max)) {
        hit_record left_rec, right_rec;
        bool hit_left  = left->hit( r, t_min, t_max, left_rec);
        bool hit_right = right->hit(r, t_min, t_max, right_rec);
        if (hit_left && hit_right) {
            if (left_rec.t < right_rec.t)
                rec = left_rec;
            else
                rec = right_rec;
            return true;
        }
        else if (hit_left) {
            rec = left_rec;
            return true;
        }
        else if (hit_right) {
            rec = right_rec;
            return true;
        }
        else return false;
    }
    else return false;
}

int box_x_compare(const void* a, const void* b) {
    aabb boxLeft, boxRight;
    hitable *ah = *(hitable**)a;
    hitable *bh = *(hitable**)b;
    if (!ah->boundingBox(0, 0, boxLeft) || !bh->boundingBox(0, 0, boxRight))
        std::cerr << "this primitive has no bounding box (e.g. infinite plane)" << std::endl;
    if (boxLeft.min().x() - boxRight.min().x() < 0.0f)
        return -1;
    else return 1;
}

int box_y_compare(const void* a, const void* b) {
    aabb boxLeft, boxRight;
    hitable *ah = *(hitable**)a;
    hitable *bh = *(hitable**)b;
    if (!ah->boundingBox(0, 0, boxLeft) || !bh->boundingBox(0, 0, boxRight))
        std::cerr << "this primitive has no bounding box (e.g. infinite plane)" << std::endl;
    if (boxLeft.min().y() - boxRight.min().y() < 0.0f)
        return -1;
    else return 1;
}

int box_z_compare(const void* a, const void* b) {
    aabb boxLeft, boxRight;
    hitable *ah = *(hitable**)a;
    hitable *bh = *(hitable**)b;
    if (!ah->boundingBox(0, 0, boxLeft) || !bh->boundingBox(0, 0, boxRight))
        std::cerr << "this primitive has no bounding box (e.g. infinite plane)" << std::endl;
    if (boxLeft.min().z() - boxRight.min().z() < 0.0f)
        return -1;
    else return 1;
}

bvh_node::bvh_node(hitable **l, int n, float time0, float time1) {
    // sort primitives with random axis
    int axis = int(drand48() * 3);
    if (axis == 0) qsort(l, n, sizeof(hitable*), box_x_compare);
    else if (axis == 1) qsort(l, n, sizeof(hitable*), box_y_compare);
    else qsort(l, n, sizeof(hitable*), box_z_compare);

    if (n == 1)
        left = right = l[0];
    else if (n == 2) {
        left  = l[0];
        right = l[1];
    }
    else {
        left  = new bvh_node(l,     n/2,   time0, time1);
        right = new bvh_node(l+n/2, n-n/2, time0, time1);
    }
    aabb boxLeft, boxRight;
    if (!left->boundingBox(time0, time1, boxLeft) || !right->boundingBox(time0, time1, boxRight))
        std::cerr << "this primitive has no bounding box (e.g. infinite plane)" << std::endl;
    box = surroundingBox(boxLeft, boxRight);
}