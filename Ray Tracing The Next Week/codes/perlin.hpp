#pragma once
#include <random>

class perlin{
public:
    float noise(const vec3& p) const {
        // decimal
        float u = p.x() - floor(p.x());
        float v = p.y() - floor(p.y());
        float w = p.z() - floor(p.z());
        // cubic Hermite spline
        int i = floor(p.x());
        int j = floor(p.y());
        int k = floor(p.z());
        vec3 c[2][2][2];
        for (int di=0; di<2; ++di)
            for (int dj=0; dj<2; ++dj)
                for (int dk=0; dk<2; ++dk)
                    c[di][dj][dk] = randVec[perm_x[(i+di) & 255] ^
                                            perm_y[(j+dj) & 255] ^
                                            perm_z[(k+dk) & 255]];
        return interpolate(c, u, v, w);
    }

    static float interpolate(vec3 c[2][2][2], float u, float v, float w);
    float turbulence(const vec3& p, int depth = 7) const;
    static vec3* generate();
    static void permute(int *p, int n);
    static int* generate_shuffle();

    static vec3 *randVec;
    static int *perm_x;
    static int *perm_y;
    static int *perm_z;
};


inline float perlin::interpolate(vec3 c[2][2][2], float u, float v, float w) {
    float uu = u * u * (3 - 2 * u);
    float vv = v * v * (3 - 2 * v);
    float ww = w * w * (3 - 2 * w);
    float sum = 0;
    for (int i=0; i<2; ++i)
        for (int j = 0; j < 2; ++j)
            for (int k = 0; k < 2; ++k) {
                vec3 weight(u - i, v - j, w - k);
                sum += (i * uu + (1 - i) * (1 - uu))
                       * (j * vv + (1 - j) * (1 - vv))
                       * (k * ww + (1 - k) * (1 - ww))
                       * dot(c[i][j][k], weight);
            }
    return sum;
}

float perlin::turbulence(const vec3& p, int depth) const {
    float sum = 0;
    vec3 temp = p;
    float weight = 1.0;
    for (int i=0; i<depth; ++i) {
        sum += weight * noise(temp);
        weight *= 0.5;
        temp *= 2;
    }
    return fabs(sum);
}

vec3* perlin::generate() {
    // random (-1~1)
    vec3 *p = new vec3[256];
    for (int i=0; i<256; ++i)
        p[i] = unit_vector(vec3(2*drand48()-1, 2*drand48()-1, 2*drand48()-1));
    return p;
}

void perlin::permute(int *p, int n) {
    // shuffle
    for (int i=n-1; i>0; --i) {
        int target = int(drand48() * (i + 1));
        std::swap(p[i], p[target]);
    }
}

int* perlin::generate_shuffle() {
    // shuffled [0~255]
    int *p = new int[256];
    for (int i=0; i<256; ++i)
        p[i] = i;
    permute(p, 256);
    return p;
}

vec3* perlin::randVec = perlin::generate();
int* perlin::perm_x = perlin::generate_shuffle();
int* perlin::perm_y = perlin::generate_shuffle();
int* perlin::perm_z = perlin::generate_shuffle();
