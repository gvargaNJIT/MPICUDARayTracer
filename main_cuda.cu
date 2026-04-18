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
#include <stdio.h>
#include <curand_kernel.h>

#define HOT_NODES 64

const double aspect_ratio = 16.0 / 9.0;
const int image_width = 400;
const int image_height = static_cast<int>(image_width / aspect_ratio);
const int samples_per_pixel = 10;

const int block_size = 16;

__global__ void ray_color(camera* cam, bvh* nodes, triangle* world, int num_nodes, int num_triangles, material* materials, vec3* d_image, int depth) {
    curandState state;
    int col = threadIdx.x+blockDim.x*blockIdx.x;
    int row = threadIdx.y+blockDim.y*blockIdx.y;
    int dep = threadIdx.z+blockDim.z*blockIdx.z;
    int tid = threadIdx.y * blockDim.x + threadIdx.x;

    __shared__ char s_cam_buf[sizeof(camera)];
    __shared__ bvh s_nodes[HOT_NODES];
    camera* s_cam = reinterpret_cast<camera*>(s_cam_buf);

    if (tid == 0)
        *s_cam = *cam;
    __syncthreads();

    int nodes_to_cache = min(HOT_NODES, num_nodes);
    if (tid < nodes_to_cache)
        s_nodes[tid] = nodes[tid];

    __syncthreads();

    if (col >= image_width || row >= image_height){
        return;
    }

    int idx = dep * image_height * image_width + row * image_width + col;
    curand_init(1234, idx, 0, &state);

    vec3 result = color(0,0,0);
    double u = (col + curand_uniform(&state)) / (image_width - 1);
    double v = (row + curand_uniform(&state)) / (image_height - 1);
    ray r = s_cam->get_ray(u, v);

    vec3 attenuation = color(1.0, 1.0, 1.0);

    for (int i = 0; i < depth; i++) {
        hit_record rec;
        
        if (bvh_hit(r, 0.001, 1e30, rec, nodes, s_nodes, nodes_to_cache, world)) {
            ray scattered;
            color bounce_attenuation;
            color emitted_light = emitted(materials[rec.material_id]);
            result += attenuation * emitted_light;

            if (scatter(materials[rec.material_id], r, rec.p, rec.normal, bounce_attenuation, scattered, &state)) {
                attenuation *= bounce_attenuation;
                r = scattered;
            } else {
                break;
            }
        } else {
            vec3 unit_dir = unit_vector(r.direction());
            double t = 0.5*(unit_dir.y() + 1.0);
            result += attenuation * ((1.0-t)*color(1.0,1.0,1.0) + t*color(0.5,0.7,1.0));
            break;
        }
    }
    d_image[idx] = result;
}

int main() {
    std::vector<material> materials;

    point3 lookfrom(0,0,5);
    point3 lookat(0,0,0);
    vec3 vup(0,1,0);
    double vfov = 40.0;

    camera cam(lookfrom, lookat, vup, vfov, aspect_ratio);

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

    std::vector<bvh> h_nodes;
    build(world.objects, 0, world.objects.size(), h_nodes);

    triangle* h_triangles = world.objects.data();
    int num_triangles = world.objects.size();

    material* h_materials = materials.data();

    vec3* d_image;
    triangle* d_triangles;
    material* d_materials;
    camera* d_cam;
    vec3* h_image = new vec3[image_width * image_height * samples_per_pixel];
    bvh* d_nodes;

    cudaMalloc(&d_nodes, h_nodes.size() * sizeof(bvh));
    cudaMalloc(&d_image, image_height*image_width*samples_per_pixel * sizeof(vec3));
    cudaMalloc(&d_triangles, num_triangles * sizeof(triangle));
    cudaMalloc(&d_materials, materials.size() * sizeof(material));
    cudaMalloc(&d_cam, sizeof(camera));

    cudaMemcpy(d_triangles, world.objects.data(), num_triangles * sizeof(triangle), cudaMemcpyHostToDevice);
    cudaMemcpy(d_nodes, h_nodes.data(), h_nodes.size() * sizeof(bvh), cudaMemcpyHostToDevice);
    cudaMemcpy(d_materials, h_materials, materials.size() * sizeof(material), cudaMemcpyHostToDevice);
    cudaMemcpy(d_cam, &cam, sizeof(camera), cudaMemcpyHostToDevice);

    dim3 block(block_size, block_size, 1);
    dim3 grid((image_width + block.x-1)/block.x, (image_height + block.y-1)/block.y, samples_per_pixel);

    ray_color<<<grid, block>>>(d_cam, d_nodes, d_triangles, h_nodes.size(), num_triangles, d_materials, d_image, 4);

    cudaMemcpy(h_image, d_image, image_height*image_width*samples_per_pixel * sizeof(vec3), cudaMemcpyDeviceToHost);

    std::vector<unsigned char> image_data;
    image_data.reserve(image_width * image_height * 3);

    for(int row = image_height-1; row >= 0; row--){
        for(int col = 0; col < image_width; col++){
            int i = row * image_width + col;
            vec3 output(0,0,0);
            for(int j=0; j<samples_per_pixel; j++){
                int offset = i + j * image_height*image_width;
                output += h_image[offset];
            }
            output = output/samples_per_pixel;
            output = color(sqrt(output.x()), sqrt(output.y()), sqrt(output.z()));

            auto clamp = [](double x, double min, double max) {
                return x < min ? min : (x > max ? max : x);
            };

            image_data.push_back(static_cast<unsigned char>(255.999 * clamp(output.x(), 0.0, 1.0)));
            image_data.push_back(static_cast<unsigned char>(255.999 * clamp(output.y(), 0.0, 1.0)));
            image_data.push_back(static_cast<unsigned char>(255.999 * clamp(output.z(), 0.0, 1.0)));
        }
    }
    image_ppm("output.ppm", image_data, image_width, image_height);

    cudaFree(d_image);
    cudaFree(d_triangles);
    cudaFree(d_materials);
    cudaFree(d_cam);
    cudaFree(d_nodes);
    delete[] h_image;
}