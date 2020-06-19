#include "color.hpp"
#include "hittable_list.hpp"
#include "sphere.hpp"
#include "moving_sphere.hpp"
#include "camera.hpp"
#include "common.hpp"
#include "material.hpp"
#include "box.hpp"
#include "constant_medium.hpp"

color ray_color(const ray& r, const color& background, const hittable& world, int depth) {
    hit_record rec;
    if (depth <= 0)
        return color(0, 0, 0);

    if (!world.hit(r, 0.001, infinity, rec))
        return background;

    ray scattered;
    color attenuation;
    color emitted = rec.mat->emitted(rec.u, rec.v, rec.p);

    if (!rec.mat->scatter(r, rec, attenuation, scattered))
        return emitted;

    return emitted + attenuation * ray_color(scattered, background, world, depth - 1);
}


hittable_list two_spheres() {
    hittable_list objects;

    auto checker1 = make_shared<checker_texture>(
            make_shared<solid_color>(0.9, 0.9, 0.9),
            make_shared<solid_color>(0.9, 0.3, 0.2)
            );
    auto checker2 = make_shared<checker_texture>(
            make_shared<solid_color>(0.3, 0.6, 1.0),
            make_shared<solid_color>(0.9, 0.9, 0.9)
    );

    objects.add(make_shared<sphere>(point3(0, -4, -3), 5, make_shared<lambertian>(checker1)));
    objects.add(make_shared<sphere>(point3(0, 4, 3), 5, make_shared<lambertian>(checker2)));

    return objects;
}

hittable_list two_perlin_spheres() {
    hittable_list objects;

    auto pertext = make_shared<noise_texture>(5);
    objects.add(make_shared<sphere>(point3(0, -1000, 0), 1000, make_shared<lambertian>(pertext)));
    objects.add(make_shared<sphere>(point3(0, 2, 0), 2, make_shared<lambertian>(pertext)));

    return objects;
}

hittable_list earth() {
    auto earth_texture = make_shared<image_texture>("earthmap.jpg");
    auto earth_surface = make_shared<lambertian>(earth_texture);
    auto globe = make_shared<sphere>(point3(0, 0, 0), 2, earth_surface);
    return hittable_list(globe);
}

hittable_list simple_light() {
    hittable_list objects;

    auto pertext = make_shared<noise_texture>(4);
    objects.add(make_shared<sphere>(point3(0, -1000, 0), 1000, make_shared<lambertian>(pertext)));
    objects.add(make_shared<sphere>(point3(0, 2, 0), 2, make_shared<lambertian>(pertext)));

    auto arealight = make_shared<area_light>(make_shared<solid_color>(4, 4, 4));
    objects.add(make_shared<xy_rect>(3, 5, 1, 3, -2, arealight));

    return objects;
}

hittable_list cornell_box() {
    hittable_list objects;
    auto red = make_shared<lambertian>(make_shared<solid_color>(0.65, 0.05, 0.05));
    auto white = make_shared<lambertian>(make_shared<solid_color>(0.73, 0.73, 0.73));
    auto green = make_shared<lambertian>(make_shared<solid_color>(0.12, 0.45, 0.15));
    auto light = make_shared<area_light>(make_shared<solid_color>(15, 15, 15));

    objects.add(make_shared<flip_face>(make_shared<yz_rect>(0, 555, 0, 555, 555, green)));
    objects.add(make_shared<yz_rect>(0, 555, 0, 555, 0, red));
    objects.add(make_shared<xz_rect>(213, 343, 227, 332, 554, light));
    objects.add(make_shared<flip_face>(make_shared<xz_rect>(0, 555, 0, 555, 0, white)));
    objects.add(make_shared<xz_rect>(0, 555, 0, 555, 555, white));
    objects.add(make_shared<flip_face>(make_shared<xy_rect>(0, 555, 0, 555, 555, white)));

    shared_ptr<hittable> box1 = make_shared<box>(point3(0, 0, 0), point3(165, 330, 165), white);
    box1 = make_shared<rotate_y>(box1, 15);
    box1 = make_shared<translate>(box1, vec3(265, 0, 295));
    objects.add(box1);

    shared_ptr<hittable> box2 = make_shared<box>(point3(0, 0, 0), point3(165, 165, 165), white);
    box2 = make_shared<rotate_y>(box2, -18);
    box2 = make_shared<translate>(box2, vec3(130, 0, 65));
    objects.add(box2);

    return objects;
}

hittable_list cornell_smoke() {
    hittable_list objects;
    auto red = make_shared<lambertian>(make_shared<solid_color>(0.65, 0.05, 0.05));
    auto white = make_shared<lambertian>(make_shared<solid_color>(0.73, 0.73, 0.73));
    auto green = make_shared<lambertian>(make_shared<solid_color>(0.12, 0.45, 0.15));
    auto light = make_shared<area_light>(make_shared<solid_color>(15, 15, 15));

    objects.add(make_shared<flip_face>(make_shared<yz_rect>(0, 555, 0, 555, 555, green)));
    objects.add(make_shared<yz_rect>(0, 555, 0, 555, 0, red));
    objects.add(make_shared<xz_rect>(213, 343, 227, 332, 554, light));
    objects.add(make_shared<flip_face>(make_shared<xz_rect>(0, 555, 0, 555, 0, white)));
    objects.add(make_shared<xz_rect>(0, 555, 0, 555, 555, white));
    objects.add(make_shared<flip_face>(make_shared<xy_rect>(0, 555, 0, 555, 555, white)));

    shared_ptr<hittable> box1 = make_shared<box>(point3(0, 0, 0), point3(165, 330, 165), white);
    box1 = make_shared<rotate_y>(box1, 15);
    box1 = make_shared<translate>(box1, vec3(265, 0, 295));
    box1 = make_shared<constant_medium>(box1, 0.01, make_shared<solid_color>(0, 0, 0));
    objects.add(box1);

    shared_ptr<hittable> box2 = make_shared<box>(point3(0, 0, 0), point3(165, 165, 165), white);
    box2 = make_shared<rotate_y>(box2, -18);
    box2 = make_shared<translate>(box2, vec3(130, 0, 65));
    box2 = make_shared<constant_medium>(box2, 0.01, make_shared<solid_color>(1, 1, 1));
    objects.add(box2);

    return objects;
}


int main() {
    const auto aspect_ratio = 16.0 / 9.0;
    const int image_width = 384;
    const int image_height = static_cast<int>(image_width / aspect_ratio);
    const int samples_per_pixel = 500;
    const int max_depth = 50;
    const color background(0, 0, 0);

    std::cout << "P3\n" << image_width << ' ' << image_height << "\n255\n";

    hittable_list world = cornell_smoke();

    point3 look_from(278, 278, -800);
    point3 look_at(278, 278, 0);
    camera cam(look_from, look_at, vec3(0,1,0), 40, aspect_ratio, 0.0, 10, 0.0, 1.0);

    // viewport - 3.6 : 2.0
    // image - 384 : 216
    for (int j=image_height-1; j>=0; --j) {
        std::cerr << '\r' << (image_height-j)*100/image_height  << '%' << std::flush;
        for (int i=0; i<image_width; ++i) {
             color pixel_color(0, 0, 0);
            for (int s=0; s<samples_per_pixel; ++s) {
                auto u = (i + random_double()) / (image_width - 1);
                auto v = (j + random_double()) / (image_height - 1);
                ray r = cam.get_ray(u, v);
                pixel_color += ray_color(r, background, world, max_depth);
            }
            write_color(std::cout, pixel_color, samples_per_pixel);
        }
    }
    std::cerr << "\nDone\n";
}