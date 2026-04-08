#pragma once
#include "ray.h"
#include "vec3.h"

#ifdef __CUDACC__
#define HD __host__ __device__
#else
#define HD
#endif

enum class materialType { LAMBERT, METAL, EMISSIVE };

struct material {
    materialType type;
    vec3 albedo;
    double fuzz;
    vec3 emit_color;
};

HD inline bool scatter(const material& mat, const ray& r_in, const vec3& hit_point, const vec3& normal, vec3& attenuation, ray& scattered){
    switch(mat.type) {
        case materialType::LAMBERT: {
            vec3 scatter_dir = normal + random_unit_vector();
            if(scatter_dir.near_zero()){
                scatter_dir = normal;
            }
            scattered = ray(hit_point, scatter_dir);
            attenuation = mat.albedo;
            return true;
        }
        case materialType::METAL: {
            vec3 reflected = reflect(unit_vector(r_in.direction()), normal);
            scattered = ray(hit_point, reflected + mat.fuzz*random_unit_vector());
            attenuation = mat.albedo;
            return (dot(scattered.direction(), normal) > 0);
        }
        case materialType::EMISSIVE: {
            return false;
        }
    }
    return false;
}

HD inline vec3 emitted(const material& mat) {
    if (mat.type == materialType::EMISSIVE)
        return mat.emit_color;
    return vec3(0, 0, 0);
}