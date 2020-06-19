#pragma once
#include "common.hpp"

class camera{
public:
    camera(
            point3 look_from,
            point3 look_at,
            vec3 v_up,
            double vertical_field,
            double aspect_ratio,
            double aperture,
            double focus_dist,
            double t0 = 0,
            double t1 = 0
            ) {

        auto theta = degree_to_radian(vertical_field);
        auto h = tan(theta/2);

        auto viewport_height = 2.0 * h;
        auto viewport_width = aspect_ratio * viewport_height;

        w = unit_vector(look_from - look_at);
        u = unit_vector(cross(v_up, w));
        v = cross(w, u);

        origin = look_from;
        horizontal = focus_dist * viewport_width * u;
        vertical = focus_dist * viewport_height * v;

        // projection plane's lower left corner
        lower_left_corner = origin - horizontal / 2 - vertical / 2 - w * focus_dist;

        lens_radius = aperture / 2;

        time0 = t0;
        time1 = t1;
    }

    ray get_ray(double s, double t) const {
        vec3 rd = lens_radius * random_in_unit_disk();
        vec3 offset = rd.x() * u + rd.y() * v;
        return ray(origin + offset, lower_left_corner + s * horizontal + t * vertical - origin - offset,
                random_double(time0, time1));
    }

private:
    point3 origin;
    point3 lower_left_corner;
    vec3 horizontal;
    vec3 vertical;
    vec3 u, v, w;
    double lens_radius;
    double time0, time1;
};