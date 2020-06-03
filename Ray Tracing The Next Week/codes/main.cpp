#include <iostream>
#include <fstream>
#include "sphere.hpp"
#include "moving_sphere.hpp"
#include "hitable_list.hpp"
#include "material.hpp"
#include "camera.hpp"

vec3 color(const ray& r, hitable *world, int depth) {
    hit_record rec;
    if (world->hit(r, 0.001, MAXFLOAT, rec)) {
        ray scattered;
        vec3 attenuation;
        if (depth < 50 && rec.mat->scatter(r, rec, attenuation, scattered))
            return attenuation * color(scattered, world, depth+1);
        else
            return vec3(0, 0, 0);
    }
    else {
        vec3 unit_direction = unit_vector(r.direction());
        float t = 0.5 * (unit_direction.y() + 1.0);
        return (1 - t) * vec3(1.0, 1.0, 1.0) + t * vec3(0.5, 0.7, 1.0);
    }
}

hitable* random_scene() {
    int n = 100;
    hitable** list = new hitable*[n+1];
    texture *checker = new checker_texture(
            new constant_texture(vec3(0.2, 0.3, 0.1)), new constant_texture(vec3(0.9, 0.9, 0.9)));
    list[0] = new sphere(vec3(0, -1000, 0), 1000, new lambertian(checker));
    int i = 1;
    for (int a=-3; a<6; ++a) {
        for (int b=-5; b<5; ++b) {
            float choose_mat = drand48();
            vec3 center(a+0.9*drand48(), 0.2, b+0.9*drand48());
            if ((center-vec3(4,0,2.0)).length() > 0.9) {
                if (choose_mat < 0.8)
                    list[i++] = new moving_sphere(center, center+vec3(0, 0.5*drand48(), 0), 0.0, 1.0, 0.2,
                            new lambertian(new constant_texture(vec3(drand48()*drand48(), drand48()*drand48(), drand48()*drand48()))));
                else if (choose_mat < 0.95)
                    list[i++] = new sphere(center, 0.2,
                            new metal(vec3(0.5*(1+drand48()), 0.5*(1+drand48()), 0.5*drand48()), drand48()));
                else
                    list[i++] = new sphere(center, 0.2,
                            new dielectric(1.5));
            }
        }
    }
    list[i++] = new sphere(vec3(0, 1, 0), 1.0, new dielectric(1.5));
    list[i++] = new sphere(vec3(-4, 1, 0),1.0, new lambertian(new constant_texture(vec3(0.4, 0.2, 0.1))));
    list[i++] = new sphere(vec3(4, 1, 0), 1.0, new metal(vec3(0.7, 0.6, 0.5), 0.0));
    return new hitable_list(list, i);
}

hitable* two_spheres() {
    texture *checker1 = new checker_texture(
            new constant_texture(vec3(0.9, 0.3, 0.2)), new constant_texture(vec3(1, 1, 1)));
    texture *checker2 = new checker_texture(
            new constant_texture(vec3(0.3, 0.6, 1)), new constant_texture(vec3(1, 1, 1)));
    int n = 50;
    hitable **list = new hitable*[n+1];
    list[0] = new sphere(vec3(0, -4, -3), 5, new lambertian(checker1));
    list[1] = new sphere(vec3(0, 4, 3), 5, new lambertian(checker2));
    return new hitable_list(list, 2);
}

hitable* two_perlin_spheres() {
    texture *perlin = new noise_texture(5);
    hitable **list = new hitable*[2];
    list[0] = new sphere(vec3(0, -1000, 0), 1000, new lambertian(perlin));
    list[1] = new sphere(vec3(0, 2, 0), 2, new lambertian(perlin));
    return new hitable_list(list, 2);
}

int main()
{
    int nx = 400;
    int ny = 200;
    int ns = 100;
    std::ofstream file;
    file.open("perlin.ppm");
    file << "P3\n" << nx << ' ' << ny << "\n255\n";

    hitable *world = two_perlin_spheres();

    vec3 lookfrom(13,2,3);
    vec3 lookat(0,0,0);
    float focus = 10;
    camera cam(lookfrom, lookat, vec3(0, 1, 0), 20, float(nx)/float(ny), 0, focus, 0.0, 1.0);

    for (int j=ny-1; j>=0; --j) {
        for (int i=0; i<nx; ++i) {
            vec3 col(0,0,0);
            for (int s=0; s<ns; ++s) {
                float u = float(i+drand48()) / float(nx);
                float v = float(j+drand48()) / float(ny);
                ray r = cam.get_ray(u, v);
                col += color(r, world, 0);
            }
            col /= float(ns);
            if (col[0]<0 || col[1]<0 || col[2]<0) std::cerr << "color error";
            col = vec3(sqrt(col[0]), sqrt(col[1]), sqrt(col[2]));
            int ir = int(255.99*col[0]);
            int ig = int(255.99*col[1]);
            int ib = int(255.99*col[2]);
            file << ir << ' ' << ig << ' ' << ib << '\n';
        }
    }
    file.close();
}