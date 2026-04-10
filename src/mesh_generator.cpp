#include "../include/mesh_generator.h"
#include <cmath>

std::vector<triangle> mesh_generator::generate_sphere(const vec3& center, double radius, int lat_steps, int long_steps, int material_id){
    std::vector<vec3> vertices;
    std::vector<triangle> tris;
    std::vector<vec3> normals;

    for (int i = 0; i <= lat_steps; ++i) {
        double theta = i * M_PI / lat_steps;
        for (int j = 0; j < long_steps; ++j) {
            double phi = j * 2 * M_PI / long_steps;
            double x = radius * sin(theta) * cos(phi);
            double y = radius * cos(theta);
            double z = radius * sin(theta) * sin(phi);
            vertices.push_back(center + vec3(x, y, z));
            normals.push_back(unit_vector(vec3(x, y, z)));
        }
    }

    int row_stride = long_steps;

    for (int i = 0; i < lat_steps; ++i) {
        for (int j = 0; j < long_steps; ++j) {

            int next_j = (j + 1) % long_steps;

            int first  = i * row_stride + j;
            int second = (i + 1) * row_stride + j;
            int first_next  = i * row_stride + next_j;
            int second_next = (i + 1) * row_stride + next_j;

            tris.push_back(triangle(vertices[first], vertices[second], vertices[first_next], normals[first], normals[second], normals[first_next], material_id));

            tris.push_back(triangle(vertices[second], vertices[second_next], vertices[first_next], normals[second], normals[second_next], normals[first_next], material_id));
    }
}

    return tris;
}

std::vector<triangle> mesh_generator::generate_cube(const vec3& center, double side, int material_id) {
    std::vector<triangle> tris;
    double h = side / 2.0;

    point3 v[8] = {
        center + vec3(-h, -h, -h),
        center + vec3( h, -h, -h),
        center + vec3( h,  h, -h),
        center + vec3(-h,  h, -h),
        center + vec3(-h, -h,  h),
        center + vec3( h, -h,  h),
        center + vec3( h,  h,  h),
        center + vec3(-h,  h,  h)
    };

    vec3 nx_pos(1, 0, 0), nx_neg(-1, 0, 0);
    vec3 ny_pos(0, 1, 0), ny_neg(0, -1, 0);
    vec3 nz_pos(0, 0, 1), nz_neg(0, 0, -1);

    //Z+
    tris.push_back(triangle(v[4], v[5], v[6], nz_pos, nz_pos, nz_pos, material_id));
    tris.push_back(triangle(v[4], v[6], v[7], nz_pos, nz_pos, nz_pos, material_id));
    //Z-
    tris.push_back(triangle(v[1], v[0], v[3], nz_neg, nz_neg, nz_neg, material_id));
    tris.push_back(triangle(v[1], v[3], v[2], nz_neg, nz_neg, nz_neg, material_id));
    //Y+
    tris.push_back(triangle(v[3], v[2], v[6], ny_pos, ny_pos, ny_pos, material_id));
    tris.push_back(triangle(v[3], v[6], v[7], ny_pos, ny_pos, ny_pos, material_id));
    //Y-
    tris.push_back(triangle(v[0], v[1], v[5], ny_neg, ny_neg, ny_neg, material_id));
    tris.push_back(triangle(v[0], v[5], v[4], ny_neg, ny_neg, ny_neg, material_id));
    //X+
    tris.push_back(triangle(v[1], v[5], v[6], nx_pos, nx_pos, nx_pos, material_id));
    tris.push_back(triangle(v[1], v[6], v[2], nx_pos, nx_pos, nx_pos, material_id));
    //X-
    tris.push_back(triangle(v[0], v[4], v[7], nx_neg, nx_neg, nx_neg, material_id));
    tris.push_back(triangle(v[0], v[7], v[3], nx_neg, nx_neg, nx_neg, material_id));

    return tris;
}