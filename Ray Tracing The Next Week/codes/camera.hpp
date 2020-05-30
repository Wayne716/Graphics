#pragma once
#include "ray.hpp"

vec3 random_in_unit_disk() {
    vec3 p;
    do {
        p = 2.0 * vec3(drand48(), drand48(), 0) - vec3(1, 1, 0);
    } while (dot(p, p) >= 1.0);
    return p;
}

class camera {
public:
    camera(vec3 lookfrom, vec3 lookat, vec3 vup, float vert_fov, float ratio,
            float aperture, float focus, float t0, float t1) {
        time0 = t0;
        time1 = t1;
        lens_radius = aperture / 2;
        float theta = vert_fov / 180 * M_PI;
        float half_height = tan(theta/2);
        float half_width = half_height * ratio;
        origin = lookfrom;

        w = unit_vector(lookfrom - lookat);
        u = unit_vector(cross(vup, w));
        v = cross(w, u);

        lower_left_corner = origin - focus*w - focus*half_width*u - focus*half_height*v;
        horizontal = focus * half_width * 2 * u;
        vertical = focus * half_height * 2 * v;
    }

    ray get_ray(float s, float t) {
        vec3 rd = lens_radius * random_in_unit_disk();
        vec3 offset = u * rd.x() + v * rd.y();
        float time = time0 + drand48() * (time1 - time0);
        return ray(origin + offset, lower_left_corner + s*horizontal + t*vertical - (origin + offset), time);
    }

    vec3 origin;
    vec3 lower_left_corner;
    vec3 horizontal;
    vec3 vertical;
    vec3 u, v, w;
    float time0, time1;
    float lens_radius;
};