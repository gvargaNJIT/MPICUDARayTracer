#include "../include/ray.h"

#ifdef __CUDACC__
#define HD __host__ __device__
#else
#define HD
#endif

HD ray::ray() = default;

HD ray::ray(const point3& origin, const vec3& direction): orig(origin), dir(direction) {}

HD const point3& ray::origin() const { return orig; }
HD const vec3& ray::direction() const { return dir; }

HD point3 ray::at(double t) const { return orig + t*dir; }