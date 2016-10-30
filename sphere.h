//
// Created by Tracy on 8/1/2016.
//

#ifndef RAY_TRACING_SPHERE_H
#define RAY_TRACING_SPHERE_H

#include "hitable.h"
#include "material.h"

#include <memory>

class sphere : public hitable {
public:
    sphere() {}
    sphere(const vec3& cen, float r, std::unique_ptr<material> mat) : center(cen), radius(r), material_ptr(std::move(mat)) {}

    virtual bool hit(const ray &r, float t_min, float t_max, hit_record &rec) const override;

    vec3 center;
    float radius;
    std::unique_ptr<material> material_ptr;
};

#endif //RAY_TRACING_SPHERE_H
