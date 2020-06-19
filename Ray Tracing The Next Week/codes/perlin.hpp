#pragma once
#include "common.hpp"

inline double trilinear_interpolate(vec3 c[2][2][2], double u, double v, double w) {
    auto sum = 0.0;

    // Hermitian Smoothing
    u = u * u * (3 - 2 * u);
    v = v * v * (3 - 2 * v);
    w = w * w * (3 - 2 * w);

    for (int i=0; i<2; ++i)
        for (int j=0; j<2; ++j)
            for (int k=0; k<2; ++k) {
                // weight : center to lattice point
                vec3 weight(u - i, v - j, w - k);
                sum +=   (i * u + (1 - i) * (1 - u))
                       * (j * v + (1 - j) * (1 - v))
                       * (k * w + (1 - k) * (1 - w))
                       * dot(c[i][j][k], weight);
            }

    return sum;
}

class perlin {
public:
    perlin() {
        rand_vec = new vec3[point_count];
        // rand_array = new double[point_count];
        for (int i=0; i<point_count; ++i)
            // rand_array[i] = random_double();
            rand_vec[i] = unit_vector(vec3::random(-1, 1));

        perm_x = perlin_generate_perm();
        perm_y = perlin_generate_perm();
        perm_z = perlin_generate_perm();
    }

    ~perlin() {
        delete[] rand_vec;
        // delete[] rand_array;
        delete[] perm_x;
        delete[] perm_y;
        delete[] perm_z;
    }

    double turb(const point3& p, int depth=7) const {
        auto accum = 0.0;
        auto temp_p = p;
        auto weight = 1.0;

        for (int i=0; i<depth; ++i) {
            accum += weight * noise(temp_p);
            weight *= 0.5;
            temp_p *= 2;
        }

        return fabs(accum);
    }

    double noise(const point3& p) const {
        // get decimal to get into unit space
        auto u = p.x() - floor(p.x());
        auto v = p.y() - floor(p.y());
        auto w = p.z() - floor(p.z());

        int i = floor(p.x());
        int j = floor(p.y());
        int k = floor(p.z());

        vec3 c[2][2][2];
        for (int di=0; di<2; ++di)
            for (int dj=0; dj<2; ++dj)
                for (int dk=0; dk<2; ++dk)
                    c[di][dj][dk] = rand_vec[
                            perm_x[(i + di) & 255] ^
                            perm_y[(j + dj) & 255] ^
                            perm_z[(k + dk) & 255]];

        // return a gray scale
        return trilinear_interpolate(c, u, v, w);
    }

private:
    static const int point_count = 256;
    vec3* rand_vec;
    int* perm_x;
    int* perm_y;
    int* perm_z;

    static int* perlin_generate_perm() {
        auto p = new int[point_count];
        for (int i=0; i<point_count; ++i)
            p[i] = i;
        permute(p, point_count);
        return p;
    }

    // randomize
    static void permute(int* p, int n) {
        for (int i=n-1; i>0; --i) {
            int target = random_int(0, i);
            int tmp = p[i];
            p[i] = p[target];
            p[target] = tmp;
        }
    }
};