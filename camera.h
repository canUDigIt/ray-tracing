//
// Created by Tracy on 8/1/2016.
//

#ifndef RAY_TRACING_CAMERA_H
#define RAY_TRACING_CAMERA_H

#include "ray.h"

class camera {
public:
    camera()
        : lower_left_corner(-2.f, -1.f, -1.f)
        , horizontal( 4.f, 0.f, 0.f)
        , vertical(0.f, 2.f, 0.f)
        , origin(0.f, 0.f, 0.f)
    {}

    ray get_ray(float u, float v) { return ray(origin, lower_left_corner + u * horizontal + v * vertical - origin); }

    vec3 origin;
    vec3 lower_left_corner;
    vec3 horizontal;
    vec3 vertical;
};


#endif //RAY_TRACING_CAMERA_H
