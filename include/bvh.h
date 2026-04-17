#pragma once
#include "aabb.h"
#include "triangle.h"
#include "ray.h"
#include "physical.h"
#include <vector>
#include <cmath>

#ifdef __CUDACC__
#define HD __host__ __device__
#else
#define HD
#endif

struct bvh {
    aabb box;
    int left_idx;
    int right_idx;
    int triangle_idx;

    HD bool is_leaf() const { 
        return left_idx == -1; 
    }
};

inline aabb surrounding_box(const aabb& a, const aabb& b) {
    point3 small(std::fmin(a.minimum.x(), b.minimum.x()),
                 std::fmin(a.minimum.y(), b.minimum.y()),
                 std::fmin(a.minimum.z(), b.minimum.z()));
    point3 big(std::fmax(a.maximum.x(), b.maximum.x()),
               std::fmax(a.maximum.y(), b.maximum.y()),
               std::fmax(a.maximum.z(), b.maximum.z()));
    return aabb(small, big);
}

HD inline bool bvh_hit(const ray& r, double t_min, double t_max, hit_record& rec, const bvh* all_nodes, const triangle* all_tris) {
    int stack[64]; 
    int stack_ptr = 0;
    stack[stack_ptr++] = 0;

    bool hit_anything = false;
    double closest_so_far = t_max;

    while (stack_ptr > 0) {
        int idx = stack[--stack_ptr];
        const bvh& node = all_nodes[idx];

        if (node.box.hit(r, t_min, closest_so_far)) {
            if (node.is_leaf()) {
                if (all_tris[node.triangle_idx].hit(r, t_min, closest_so_far, rec)) {
                    hit_anything = true;
                    closest_so_far = rec.t;
                }
            } else {
                stack[stack_ptr++] = node.left_idx;
                stack[stack_ptr++] = node.right_idx;
            }
        }
    }
    return hit_anything;
}

int build(std::vector<triangle>& objects, int start, int end, std::vector<bvh>& flat_nodes);