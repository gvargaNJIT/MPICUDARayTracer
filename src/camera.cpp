#include "../include/camera.h"
#include <cmath>

camera::camera(point3 lookfrom, point3 lookat, vec3 vup, double vfov, double aspect_ratio) {
    auto theta = vfov * M_PI / 180.0;
    auto h = tan(theta/2);
    auto viewport_height = 2.0 * h;
    auto viewport_width = aspect_ratio * viewport_height;

    origin = lookfrom;
    vec3 w = unit_vector(lookfrom - lookat);
    vec3 u = unit_vector(cross(vup, w));
    vec3 v = cross(w, u);

    horizontal = viewport_width * u;
    vertical = viewport_height * v;
    lower_left_corner = origin - horizontal/2 - vertical/2 - w;
}

ray camera::get_ray(double s, double t) const {
    return ray(origin, lower_left_corner + s*horizontal + t*vertical - origin);
}