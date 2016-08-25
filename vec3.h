//
// Created by Tracy on 7/31/2016.
//

#ifndef RAY_TRACING_VEC3_H
#define RAY_TRACING_VEC3_H

#include <iostream>
#include <cmath>

class vec3 {
public:
    vec3() {}
    vec3(float _x, float _y, float _z) : x(_x), y(_y), z(_z) {}

    inline const vec3& operator+() const { return *this; }
    inline vec3 operator-() const { return vec3(-x, -y, -z); }

    vec3& operator+=(const vec3& v2);
    vec3& operator-=(const vec3& v2);
    vec3& operator*=(const vec3& v2);
    vec3& operator/=(const vec3& v2);
    vec3& operator*=(const float t);
    vec3& operator/=(const float t);

    inline float length() const {
        return static_cast<float>( sqrt(squared_length()) );
    }
    inline float squared_length() const {
        return x*x + y*y + z*z;
    }
    void make_unit_vector();

    union { float x; float r; };
    union { float y; float g; };
    union { float z; float b; };
};

inline std::istream& operator>>(std::istream& is, vec3& t) {
    is >> t.x >> t.y >> t.z;
    return is;
}

inline std::ostream& operator<<(std::ostream& os, vec3& t) {
    os << t.x << t.y << t.z;
    return os;
}

inline vec3 operator+(const vec3& v1, const vec3& v2) {
    return vec3(v1.x + v2. x, v1.y + v2.y, v1.z + v2.z);
}

inline vec3 operator-(const vec3& v1, const vec3& v2) {
    return vec3(v1.x - v2. x, v1.y - v2.y, v1.z - v2.z);
}

inline vec3 operator*(const vec3& v1, const vec3& v2) {
    return vec3(v1.x * v2. x, v1.y * v2.y, v1.z * v2.z);
}

inline vec3 operator/(const vec3& v1, const vec3& v2) {
    return vec3(v1.x / v2. x, v1.y / v2.y, v1.z / v2.z);
}

inline vec3 operator*(const vec3& v, float t) {
    return vec3(v.x * t, v.y * t, v.z * t);
}

inline vec3 operator*(float t, const vec3& v) {
    return vec3(v.x * t, v.y * t, v.z * t);
}

inline vec3 operator/(const vec3& v, float t) {
    return vec3(v.x / t, v.y / t, v.z / t);
}

inline float dot(const vec3& v1, const vec3& v2) {
    return v1.x*v2.x + v1.y*v2.y + v1.z*v2.z;
}

inline vec3 cross(const vec3& v1, const vec3& v2) {
    return vec3(
        v1.y*v2.z - v1.z*v2.y,
        -(v1.x*v2.z - v1.z*v2.x),
        v1.x*v2.y - v1.y*v2.x
    );
}

inline vec3 unit_vector(const vec3& v) {
    return v / v.length();
}

inline bool refract(const vec3& v, const vec3& n, float ni_over_nt, vec3& refracted) {
    vec3 uv = unit_vector(v);
    float dt = dot(uv, n);
    float discriminant = 1.f - ni_over_nt*ni_over_nt*(1-dt*dt);
    if (discriminant > 0.f) {
        refracted = ni_over_nt*(uv - n*dt) - n*sqrt(discriminant);
        return true;
    }
    else
        return false;
}

inline vec3 reflect(const vec3& v, const vec3& n) {
    return v - 2*dot(v, n)*n;
}

inline vec3 random_in_unit_sphere() {
    vec3 p;
    do {
        p = 2.f*vec3(drand48(), drand48(), drand48()) - vec3(1.f, 1.f, 1.f);
    } while(p.squared_length() >= 1.f);
    return p;
}

#endif //RAY_TRACING_VEC3_H
