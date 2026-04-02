#pragma once
#include "material.h"

class hit_record;

class lambert : public material {
public:
    lambert(const color& albedo);

    bool scatter(const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered) const override;

private:
    color albedo;
};