#pragma once
#include "physical.h"
#include <memory>
#include <vector>

using std::make_shared;
using std::shared_ptr;

class physical_list : public physical {
  public:
    std::vector<shared_ptr<physical>> objects;

    physical_list() {}
    physical_list(shared_ptr<physical> object) { add(object); }

    void clear() { objects.clear(); }

    void add(shared_ptr<physical> object) {
        objects.push_back(object);
    }

    bool hit(const ray& r, double ray_tmin, double ray_tmax, hit_record& rec) const override {
        hit_record temp_rec;
        bool hit_anything = false;
        auto closest_so_far = ray_tmax;

        for (const auto& object : objects) {
            if (object->hit(r, ray_tmin, closest_so_far, temp_rec)) {
                hit_anything = true;
                closest_so_far = temp_rec.t;
                rec = temp_rec;
            }
        }

        return hit_anything;
    }
};