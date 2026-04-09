#include "../include/triangle.h"
#include <cmath>

#ifdef __CUDACC__
#define HD __host__ __device__
#else
#define HD
#endif

triangle::triangle(const point3& a, const point3& b, const point3& c, int m)
    : v0(a), v1(b), v2(c), material_id(m) {
    normal = unit_vector(cross(v1 - v0, v2 - v0));
}

HD bool triangle::hit(const ray& r, double t_min, double t_max, hit_record& rec) const{
    vec3 edge1 = v1 - v0;
    vec3 edge2 = v2 - v0;
    vec3 h = cross(r.direction(), edge2);
    double a = dot(edge1, h);
    if (fabs(a) < 1e-8){
      return false;
    }

    double f = 1.0 / a;
    vec3 s = r.origin() - v0;
    double u = f * dot(s, h);
    if (u < 0.0 || u > 1.0){
      return false;
    }

    vec3 q = cross(s, edge1);
    double v = f * dot(r.direction(), q);
    if (v < 0.0 || u + v > 1.0){
      return false; 
    }

    double t = f * dot(edge2, q);
    if (t < t_min || t > t_max){
      return false;  
    }

    rec.t = t;
    rec.p = r.at(t);
    rec.normal = unit_vector(cross(edge1, edge2));
    rec.set_face_normal(r, rec.normal);
    rec.material_id = material_id;

    return true;
}