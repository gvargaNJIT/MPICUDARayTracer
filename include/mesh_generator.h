#pragma once
#include "triangle.h"
#include "vec3.h"
#include "material.h"
#include <vector>

class mesh_generator {
  public:
    static std::vector<triangle> generate_sphere(const vec3& center, double radius, int lat_steps, int long_steps, int material_id);
};