#include "../include/mesh_generator.h"
#include <cmath>
#include <memory>

std::vector<std::shared_ptr<triangle>> mesh_generator::generate_sphere(const vec3& center, double radius, int lat_steps, int long_steps, std::shared_ptr<material> mat){
    std::vector<vec3> vertices;
    std::vector<std::shared_ptr<triangle>> tris;

    for (int i = 0; i <= lat_steps; ++i) {
        double theta = i * M_PI / lat_steps;
        for (int j = 0; j <= long_steps; ++j) {
            double phi = j * 2 * M_PI / long_steps;
            double x = radius * sin(theta) * cos(phi);
            double y = radius * cos(theta);
            double z = radius * sin(theta) * sin(phi);
            vertices.push_back(center + vec3(x, y, z));
        }
    }

    for (int i = 0; i < lat_steps; ++i) {
        for (int j = 0; j < long_steps; ++j) {
            int first = i * (long_steps + 1) + j;
            int second = first + long_steps + 1;

            tris.push_back(std::make_shared<triangle>(vertices[first], vertices[second], vertices[first + 1], mat));
            tris.push_back(std::make_shared<triangle>(vertices[second], vertices[second + 1], vertices[first + 1], mat));
        }
    }

    return tris;
}