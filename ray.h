//
// Created by Tracy on 7/31/2016.
//

#ifndef RAY_TRACING_RAY_H
#define RAY_TRACING_RAY_H


#include "vec3.h"

class ray {
public:
    ray() {}
    ray(const vec3& a, const vec3& b): origin(a), direction(b) {}
    vec3 point_at_parameter(float t) const { return origin + t*direction; }

    vec3 origin;
    vec3 direction;
};


#endif //RAY_TRACING_RAY_H
