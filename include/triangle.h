#pragma once
#include "physical.h"
#include "material.h"
#include "vec3.h"

class triangle : public physical {
  public:
    triangle(const point3& a, const point3& b, const point3& c, std::shared_ptr<material> m);

    bool hit(const ray& r, double t_min, double t_max, hit_record& rec) const override;

  private:
    point3 v0, v1, v2;
    vec3 normal;
    std::shared_ptr<material> mat;
};