#pragma once
#include "ray.h"

#ifdef __CUDACC__
#define HD __host__ __device__
#else
#define HD
#endif

class aabb {
public:
    point3 minimum;
    point3 maximum;

    HD aabb() {}
    HD aabb(const point3& a, const point3& b) : minimum(a), maximum(b) {}

    HD bool hit(const ray& r, double t_min, double t_max) const {
        for (int a = 0; a < 3; a++) {
            double invD = 1.0 / r.direction()[a];
            double t0 = (minimum[a] - r.origin()[a]) * invD;
            double t1 = (maximum[a] - r.origin()[a]) * invD;

            if (invD < 0.0) {
                double temp = t0;
                t0 = t1;
                t1 = temp;
            }

            t_min = t0 > t_min ? t0 : t_min;
            t_max = t1 < t_max ? t1 : t_max;

            if (t_max <= t_min)
                return false;
        }
        return true;
    }
};