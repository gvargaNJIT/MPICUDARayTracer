#pragma once
#include "triangle.h"
#include "vec3.h"
#include "material.h"
#include <vector>
#include <memory>

class mesh_generator {
  public:
    static std::vector<std::shared_ptr<triangle>> generate_sphere(const vec3& center, double radius, int lat_steps, int long_steps, std::shared_ptr<material> mat);
};