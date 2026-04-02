#pragma once
#include "vec3.h"
#include "ray.h"

class camera {
  public:
    camera(point3 lookfrom, point3 lookat, vec3 vup, double vfov, double aspect_ratio);

    ray get_ray(double u, double v) const;

  private:
    point3 origin;
    vec3 horizontal;
    vec3 vertical;
    point3 lower_left_corner;
};