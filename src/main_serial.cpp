#include "../include/vec3.h"
#include "../include/ray.h"
#include "../include/physical.h"
#include "../include/physical_list.h"
#include "../include/triangle.h"
#include "../include/mesh_generator.h"
#include "../include/material.h"
#include "../include/lambert.h"
#include "../include/image_maker.h"
#include "../include/camera.h"
#include <iostream>
#include <memory>
#include <vector>
#include <algorithm>

color ray_color(const ray& r, const physical& world, int depth) {
    hit_record rec;

    if (depth <= 0)
        return color(0,0,0);

    if (world.hit(r, 0.001, std::numeric_limits<double>::infinity(), rec)) {
        ray scattered;
        color attenuation;
        if (rec.mat->scatter(r, rec, attenuation, scattered))
            return attenuation * ray_color(scattered, world, depth-1);
        return color(0,0,0);
    }

    vec3 unit_dir = unit_vector(r.direction());
    double t = 0.5*(unit_dir.y() + 1.0);
    return (1.0-t)*color(1.0, 1.0, 1.0) + t*color(0.5, 0.7, 1.0);
}

int main() {
    const double aspect_ratio = 16.0 / 9.0;
    const int image_width = 400;
    const int image_height = static_cast<int>(image_width / aspect_ratio);

    point3 lookfrom(0,0,5);
    point3 lookat(0,0,0);
    vec3 vup(0,1,0);
    double vfov = 40.0;

    camera cam(lookfrom, lookat, vup, vfov, aspect_ratio);

    auto red   = std::make_shared<lambert>(color(0.7, 0.1, 0.1));
    auto green = std::make_shared<lambert>(color(0.1, 0.7, 0.1));
    auto blue  = std::make_shared<lambert>(color(0.1, 0.1, 0.7));

    physical_list world;

    auto sphere_mesh = mesh_generator::generate_sphere(point3(0,0,0), 1.0, 50, 50, red);

    for (auto& tri : sphere_mesh) {
        world.add(tri);
    }

    auto ground_material = std::make_shared<lambert>(color(0.8, 0.8, 0.0));
    double ground_radius = 1000.0;
    point3 ground_center(0, -1001, 0);

    auto ground_mesh = mesh_generator::generate_sphere(ground_center, ground_radius, 20, 20, ground_material);

    for (auto& tri : ground_mesh) {
        world.add(tri);
    }

    std::vector<unsigned char> image_data;
    image_data.reserve(image_width * image_height * 3);

    int samples_per_pixel = 10;
    for (int j = image_height-1; j >= 0; --j) {
        for (int i = 0; i < image_width; ++i) {
            color pixel_color(0, 0, 0);

            for (int s = 0; s < samples_per_pixel; ++s) {
                double u = (i + vec3::random().x()) / (image_width - 1);
                double v = (j + vec3::random().y()) / (image_height - 1);
                ray r = cam.get_ray(u, v);
                pixel_color += ray_color(r, world, 5);
            }

            pixel_color /= double(samples_per_pixel);

            pixel_color = color(sqrt(pixel_color.x()), sqrt(pixel_color.y()), sqrt(pixel_color.z()));

            image_data.push_back(static_cast<unsigned char>(255.999 * pixel_color.x()));
            image_data.push_back(static_cast<unsigned char>(255.999 * pixel_color.y()));
            image_data.push_back(static_cast<unsigned char>(255.999 * pixel_color.z()));
    }
}
    image_ppm("output.ppm", image_data, image_width, image_height);
}