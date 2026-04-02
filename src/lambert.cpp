#include "../include/physical.h"
#include "../include/lambert.h"
#include "../include/vec3.h"
#include "../include/ray.h"
#include <cmath>

lambert::lambert(const color& albedo)
    : albedo(albedo) {}

bool lambert::scatter(const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered) const {
    vec3 scatter_direction = rec.normal + random_unit_vector();

    if (scatter_direction.near_zero())
        scatter_direction = rec.normal;

    double epsilon = 1e-4;
    scattered = ray(rec.p + epsilon * rec.normal, scatter_direction);
    
    attenuation = albedo;

    return true;
}