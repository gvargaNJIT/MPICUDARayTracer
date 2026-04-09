#pragma once
#include "physical.h"
#include "material.h"
#include "vec3.h"

#ifdef __CUDACC__
#define HD __host__ __device__
#else
#define HD
#endif

class triangle : public physical {
  public:
    triangle(const point3& a, const point3& b, const point3& c, int m);

    HD bool hit(const ray& r, double t_min, double t_max, hit_record& rec) const override;

  private:
    point3 v0, v1, v2;
    vec3 normal;
    int material_id;
};