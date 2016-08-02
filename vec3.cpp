//
// Created by Tracy on 7/31/2016.
//

#include "vec3.h"

vec3& vec3::operator+=(const vec3& v2) {
    x += v2.x;
    y += v2.y;
    z += v2.z;
    return *this;
}

vec3& vec3::operator-=(const vec3& v2) {
    x -= v2.x;
    y -= v2.y;
    z -= v2.z;
    return *this;
}

vec3& vec3::operator*=(const vec3& v2) {
    x *= v2.x;
    y *= v2.y;
    z *= v2.z;
    return *this;
}

vec3& vec3::operator/=(const vec3& v2) {
    x /= v2.x;
    y /= v2.y;
    z /= v2.z;
    return *this;
}

vec3& vec3::operator*=(const float t) {
    x *= t;
    y *= t;
    z *= t;
    return *this;
}

vec3& vec3::operator/=(const float t) {
    x /= t;
    y /= t;
    z /= t;
    return *this;
}

void vec3::make_unit_vector() {
    float k  = 1.0 / length();
    *this *= k;
}
