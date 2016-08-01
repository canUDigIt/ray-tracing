//
// Created by Tracy on 8/1/2016.
//

#ifndef RAY_TRACING_SPHERE_H
#define RAY_TRACING_SPHERE_H

#include "hitable.h"

class sphere : public hitable {
public:
    sphere() {}
    sphere(const vec3& cen, float r) : center(cen), radius(r) {}

    virtual bool hit(const ray &r, float t_min, float t_max, hit_record &rec) const override;

    vec3 center;
    float radius;
};

#endif //RAY_TRACING_SPHERE_H
