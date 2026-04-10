#pragma once
#include <cmath>
#include <iostream>
#include <cstdlib>
#include <limits>

#ifdef __CUDACC__
#include <curand_kernel.h>
#define HD __host__ __device__
#else
#define HD
#endif

class vec3 {
public:
    double e[3];

    HD vec3() : e{0,0,0} {}
    HD vec3(double e0, double e1, double e2) : e{e0, e1, e2} {}

    HD double x() const { return e[0]; }
    HD double y() const { return e[1]; }
    HD double z() const { return e[2]; }
    HD double operator[](int i) const { return e[i]; }
    HD double& operator[](int i) { return e[i]; }

    HD vec3 operator-() const { return vec3(-e[0], -e[1], -e[2]); }

    HD vec3& operator+=(const vec3& v) {
        e[0]+=v.e[0]; e[1]+=v.e[1]; e[2]+=v.e[2]; return *this;
    }
    HD vec3& operator*=(double t) {
        e[0]*=t; e[1]*=t; e[2]*=t; return *this;
    }
    HD vec3& operator*=(const vec3& v) {
        e[0]*=v.e[0]; e[1]*=v.e[1]; e[2]*=v.e[2]; return *this;
    }
    HD vec3& operator/=(double t) { return *this *= 1/t; }

    HD double length() const { return std::sqrt(length_squared()); }
    HD double length_squared() const { return e[0]*e[0] + e[1]*e[1] + e[2]*e[2]; }

    HD bool near_zero() const {
        const auto s = 1e-8;
        return (std::fabs(e[0])<s && std::fabs(e[1])<s && std::fabs(e[2])<s);
    }

#ifdef __CUDACC__
    __device__ static double random_double(curandState* state) {
        return curand_uniform(state);
    }
    __device__ static double random_double(double min, double max, curandState* state) {
        return min + (max - min) * curand_uniform(state);
    }
    __device__ static vec3 random(curandState* state) {
        return vec3(random_double(state), random_double(state), random_double(state));
    }
    __device__ static vec3 random(double min, double max, curandState* state) {
        return vec3(random_double(min, max, state), random_double(min, max, state), random_double(min, max, state));
    }
#else
    static double random_double() { return rand() / (RAND_MAX + 1.0); }
    static double random_double(double min, double max) { return min + (max - min) * random_double(); }
    static vec3 random() { return vec3(random_double(), random_double(), random_double()); }
    static vec3 random(double min, double max) { return vec3(random_double(min,max), random_double(min,max), random_double(min,max)); }
#endif
};

using point3 = vec3;
using color  = vec3;

inline std::ostream& operator<<(std::ostream &out, const vec3 &v) {
    return out << v.e[0] << ' ' << v.e[1] << ' ' << v.e[2];
}
HD inline vec3 operator+(const vec3 &u, const vec3 &v) { return vec3(u.e[0]+v.e[0], u.e[1]+v.e[1], u.e[2]+v.e[2]); }
HD inline vec3 operator-(const vec3 &u, const vec3 &v) { return vec3(u.e[0]-v.e[0], u.e[1]-v.e[1], u.e[2]-v.e[2]); }
HD inline vec3 operator*(const vec3 &u, const vec3 &v) { return vec3(u.e[0]*v.e[0], u.e[1]*v.e[1], u.e[2]*v.e[2]); }
HD inline vec3 operator*(double t, const vec3 &v) { return vec3(t*v.e[0], t*v.e[1], t*v.e[2]); }
HD inline vec3 operator*(const vec3 &v, double t) { return t*v; }
HD inline vec3 operator/(const vec3 &v, double t) { return (1/t) * v; }

HD inline double dot(const vec3 &u, const vec3 &v) { return u.e[0]*v.e[0] + u.e[1]*v.e[1] + u.e[2]*v.e[2]; }
HD inline vec3 cross(const vec3 &u, const vec3 &v) {
    return vec3(u.e[1]*v.e[2]-u.e[2]*v.e[1], u.e[2]*v.e[0]-u.e[0]*v.e[2], u.e[0]*v.e[1]-u.e[1]*v.e[0]);
}
HD inline vec3 unit_vector(const vec3 &v) { return v / v.length(); }

#ifdef __CUDACC__
__device__ inline vec3 random_unit_vector(curandState* state) {
    double a = vec3::random_double(0, 2*M_PI, state);
    double z = vec3::random_double(-1, 1, state);
    double r = std::sqrt(1 - z*z);
    return vec3(r*std::cos(a), r*std::sin(a), z);
}
__device__ inline vec3 random_in_unit_disk(curandState* state) {
    while(true) {
        auto p = vec3(vec3::random_double(-1,1,state), vec3::random_double(-1,1,state), 0);
        if(p.length_squared() < 1) return p;
    }
}
__device__ inline vec3 random_on_hemisphere(const vec3& normal, curandState* state) {
    vec3 v = random_unit_vector(state);
    return dot(v, normal) > 0.0 ? v : -v;
}
#else
inline vec3 random_unit_vector() {
    auto a = vec3::random_double(0, 2*M_PI);
    auto z = vec3::random_double(-1, 1);
    auto r = std::sqrt(1 - z*z);
    return vec3(r*std::cos(a), r*std::sin(a), z);
}
inline vec3 random_in_unit_disk() {
    while(true) {
        auto p = vec3(vec3::random_double(-1,1), vec3::random_double(-1,1), 0);
        if(p.length_squared() < 1) return p;
    }
}
inline vec3 random_on_hemisphere(const vec3& normal) {
    vec3 v = random_unit_vector();
    return dot(v, normal) > 0.0 ? v : -v;
}
#endif

HD inline vec3 reflect(const vec3& v, const vec3& n) { return v - 2*dot(v,n)*n; }
HD inline vec3 refract(const vec3& uv, const vec3& n, double etai_over_etat) {
    auto cos_theta = std::fmin(dot(-uv,n), 1.0);
    vec3 r_out_perp = etai_over_etat * (uv + cos_theta*n);
    vec3 r_out_parallel = -std::sqrt(std::fabs(1.0 - r_out_perp.length_squared())) * n;
    return r_out_perp + r_out_parallel;
}