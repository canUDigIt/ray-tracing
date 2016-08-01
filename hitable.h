//
// Created by Tracy on 8/1/2016.
//

#ifndef RAY_TRACING_HITABLE_H
#define RAY_TRACING_HITABLE_H

#include "ray.h"

struct hit_record {
    float t;
    vec3 p;
    vec3 normal;
};

class hitable {
public:
    virtual bool hit(const ray& r, float t_min, float t_max, hit_record& rec) const = 0;
};

#endif //RAY_TRACING_HITABLE_H
