#pragma once
#include "physical.h"
#include "material.h"
#include "vec3.h"
#include "aabb.h"

#ifdef __CUDACC__
#define HD __host__ __device__
#else
#define HD
#endif

class triangle {
  public:
    HD triangle() {}
    HD triangle(const point3& a, const point3& b, const point3& c, const point3& na, const point3& nb, const point3& nc, int m)
        : v0(a), v1(b), v2(c), n0(na), n1(nb), n2(nc), material_id(m) { 
        normal = unit_vector(cross(v1 - v0, v2 - v0));
        }

    HD bool hit(const ray& r, double t_min, double t_max, hit_record& rec) const {
        vec3 edge1 = v1 - v0;
        vec3 edge2 = v2 - v0;
        vec3 h = cross(r.direction(), edge2);
        double a = dot(edge1, h);
        if (fabs(a) < 1e-8) return false;

        double f = 1.0 / a;
        vec3 s = r.origin() - v0;
        double u = f * dot(s, h);
        if (u < 0.0 || u > 1.0) return false;

        vec3 q = cross(s, edge1);
        double v = f * dot(r.direction(), q);
        if (v < 0.0 || u + v > 1.0) return false;

        double t = f * dot(edge2, q);
        if (t < t_min || t > t_max) return false;

        rec.t = t;
        rec.p = r.at(t);
        rec.normal = unit_vector((1 - u - v) * n0 + u * n1 + v * n2);
        rec.set_face_normal(r, rec.normal);
        rec.material_id = material_id;
        return true;
    }

    HD aabb bounding_box() const {
      point3 min_pt(
          fmin(v0.x(), fmin(v1.x(), v2.x())),
          fmin(v0.y(), fmin(v1.y(), v2.y())),
          fmin(v0.z(), fmin(v1.z(), v2.z()))
      );

      point3 max_pt(
          fmax(v0.x(), fmax(v1.x(), v2.x())),
          fmax(v0.y(), fmax(v1.y(), v2.y())),
          fmax(v0.z(), fmax(v1.z(), v2.z()))
      );

      const double eps = 1e-4;
      max_pt = max_pt + vec3(eps, eps, eps);
      min_pt = min_pt - vec3(eps, eps, eps);

      return aabb(min_pt, max_pt);
  }

  private:
    point3 v0, v1, v2;
    point3 n0, n1, n2;
    vec3 normal;
    int material_id;
};