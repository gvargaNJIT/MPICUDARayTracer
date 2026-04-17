#include "bvh.h"
#include <algorithm>

int build(std::vector<triangle>& objects, int start, int end, std::vector<bvh>& flat_nodes) {
    int curr_idx = flat_nodes.size();
    flat_nodes.emplace_back(); 

    int count = end - start;

    if (count == 1) {
        flat_nodes[curr_idx].box = objects[start].bounding_box();
        flat_nodes[curr_idx].left_idx = -1;
        flat_nodes[curr_idx].right_idx = -1;
        flat_nodes[curr_idx].triangle_idx = start; 
    } 
    else {
        int axis = rand() % 3;

        auto comparator = [axis](const triangle& a, const triangle& b) {
            return a.bounding_box().minimum[axis] < b.bounding_box().minimum[axis];
        };

        std::sort(objects.begin() + start, objects.begin() + end, comparator);

        int mid = start + count / 2;

        int left = build(objects, start, mid, flat_nodes);
        int right = build(objects, mid, end, flat_nodes);

        flat_nodes[curr_idx].left_idx = left;
        flat_nodes[curr_idx].right_idx = right;
        flat_nodes[curr_idx].triangle_idx = -1; 
        
        flat_nodes[curr_idx].box = surrounding_box(flat_nodes[left].box, flat_nodes[right].box);
    }

    return curr_idx;
}