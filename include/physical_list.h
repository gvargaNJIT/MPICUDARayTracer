#pragma once
#include "physical.h"
#include "triangle.h"
#include <vector>

#ifdef __CUDACC__
#define HD __host__ __device__
#else
#define HD
#endif

class physical_list{
  public:
    std::vector<triangle> objects;

    physical_list() {}
    physical_list(triangle object) { add(object); }

    void clear() { objects.clear(); }

    void add(triangle object) {
        objects.push_back(object);
    }

    HD bool hit(const ray& r, double ray_tmin, double ray_tmax, hit_record& rec) const{
        hit_record temp_rec;
        bool hit_anything = false;
        auto closest_so_far = ray_tmax;

        for (const auto& object : objects) {
            if (object.hit(r, ray_tmin, closest_so_far, temp_rec)) {
                hit_anything = true;
                closest_so_far = temp_rec.t;
                rec = temp_rec;
            }
        }

        return hit_anything;
    }
};