#pragma once
#include "ray.h"
#include "material.h"

#ifdef __CUDACC__
#define HD __host__ __device__
#else
#define HD
#endif

class hit_record {
  public:
    point3 p;
    vec3 normal;
    double t;
    bool front_face;
    int material_id;
    HD void set_face_normal(const ray& r, const vec3& outward_normal) {
        front_face = dot(r.direction(), outward_normal) < 0;
        normal = front_face ? outward_normal : -outward_normal;
    }
};
