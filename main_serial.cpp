#include "include/vec3.h"
#include "include/ray.h"
#include "include/physical.h"
#include "include/physical_list.h"
#include "include/triangle.h"
#include "include/mesh_generator.h"
#include "include/material.h"
#include "include/image_maker.h"
#include "include/camera.h"
#include "include/bvh.h"
#include <iostream>
#include <memory>
#include <vector>
#include <algorithm>

color ray_color(const ray& r, const std::vector<bvh>& nodes, const std::vector<triangle>& tris, const std::vector<material>& materials, int depth) {
    hit_record rec;

    if (depth <= 0)
        return color(0,0,0);

    if (bvh_hit(r, 0.001, std::numeric_limits<double>::infinity(), rec, nodes.data(), nodes.data(), 0, tris.data())) {
        ray scattered;
        color attenuation;
        color emitted_light = emitted(materials[rec.material_id]);
        
        if (scatter(materials[rec.material_id], r, rec.p, rec.normal, attenuation, scattered))
            return emitted_light + attenuation * ray_color(scattered, nodes, tris, materials, depth-1);
        
        return emitted_light;
    }

    vec3 unit_dir = unit_vector(r.direction());
    double t = 0.5*(unit_dir.y() + 1.0);
    return (1.0-t)*color(1.0, 1.0, 1.0) + t*color(0.5, 0.7, 1.0);
}

int main() {
    const double aspect_ratio = 16.0 / 9.0;
    const int image_width = 400;
    const int image_height = static_cast<int>(image_width / aspect_ratio);
    std::vector<material> materials;

    point3 camera_positions[4] = {
        point3(0, 0, 5),
        point3(5, 0, 2),
        point3(0, 5, 2),
        point3(-5, 0, 2)
    };

    point3 lookat(0,0,0);
    vec3 vup(0,1,0);
    double vfov = 40.0;

    material red, green, blue;
    red.type   = materialType::LAMBERT;
    red.albedo = color(0.7, 0.1, 0.1);
    green.type   = materialType::EMISSIVE;
    green.emit_color = color(0, 5, 0);
    blue.type   = materialType::METAL;
    blue.albedo = color(0.1, 0.1, 0.7);
    materials.push_back(red);
    materials.push_back(green);
    materials.push_back(blue);

    physical_list world;

    auto sphere_mesh = mesh_generator::generate_sphere(point3(0,0,0), 1.0, 50, 50, 0);
    auto blue_sphere_mesh  = mesh_generator::generate_sphere(point3(2.5,0,0), 1.0, 50, 50, 2);
    auto green_sphere_mesh = mesh_generator::generate_sphere(point3(-2.5,0,0),1.0, 50, 50, 1);

    for (auto& tri : sphere_mesh)       world.add(tri);
    for (auto& tri : blue_sphere_mesh)  world.add(tri);
    for (auto& tri : green_sphere_mesh) world.add(tri);

    material ground_material;
    ground_material.type   = materialType::LAMBERT;
    ground_material.albedo = color(0.8, 0.8, 0.0);
    materials.push_back(ground_material);

    double ground_side = 2000.0;
    point3 ground_center(0, -1001, 0); 

    auto ground_mesh = mesh_generator::generate_cube(ground_center, ground_side, 3);

    for (auto& tri : ground_mesh) {
        world.add(tri);
    }

    std::vector<bvh> flat_nodes;
    build(world.objects, 0, world.objects.size(), flat_nodes);

    std::vector<unsigned char> image_data;
    image_data.reserve(image_width * image_height * 3);

    for (int frame = 0; frame < 4; frame++) {
        camera cam(camera_positions[frame], lookat, vup, vfov, aspect_ratio);
        int samples_per_pixel = 10;
        for (int j = image_height-1; j >= 0; --j) {
            for (int i = 0; i < image_width; ++i) {
                color pixel_color(0, 0, 0);

                for (int s = 0; s < samples_per_pixel; ++s) {
                    double u = (i + vec3::random().x()) / (image_width - 1);
                    double v = (j + vec3::random().y()) / (image_height - 1);
                    ray r = cam.get_ray(u, v);
                    pixel_color += ray_color(r, flat_nodes, world.objects, materials, 5);
                }

                pixel_color /= double(samples_per_pixel);

                pixel_color = color(sqrt(pixel_color.x()), sqrt(pixel_color.y()), sqrt(pixel_color.z()));

                auto clamp = [](double x, double min, double max) {
                    return x < min ? min : (x > max ? max : x);
                };

                image_data.push_back(static_cast<unsigned char>(255.999 * clamp(pixel_color.x(), 0.0, 1.0)));
                image_data.push_back(static_cast<unsigned char>(255.999 * clamp(pixel_color.y(), 0.0, 1.0)));
                image_data.push_back(static_cast<unsigned char>(255.999 * clamp(pixel_color.z(), 0.0, 1.0)));
            }
        }
    std::string filename = "output_" + std::to_string(frame) + ".ppm";
    image_ppm(filename.c_str(), image_data, image_width, image_height);
    
    image_data.clear();
    }
}