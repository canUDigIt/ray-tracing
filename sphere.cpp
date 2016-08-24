//
// Created by Tracy on 8/1/2016.
//

#include "sphere.h"

bool sphere::hit(const ray& r, float t_min, float t_max, hit_record& rec) const {
    vec3 oc = r.origin - center;
    float a = dot(r.direction, r.direction);
    float b = dot(oc, r.direction);
    float c = dot(oc, oc) - radius*radius;
    float discriminant = b*b - a*c;
    if (discriminant > 0) {
        float temp = (-b - static_cast<float>(sqrt(discriminant))) / a;
        if (temp < t_max && temp > t_min) {
            rec.t = temp;
            rec.p = r.point_at_parameter(temp);
            rec.normal = (rec.p - center) / radius;
            rec.mat_ptr = material_ptr.get();
            return true;
        }
        temp = (-b + static_cast<float>(sqrt(discriminant))) / a;
        if (temp < t_max && temp > t_min) {
            rec.t = temp;
            rec.p = r.point_at_parameter(temp);
            rec.normal = (rec.p - center) / radius;
            rec.mat_ptr = material_ptr.get();
            return true;
        }
    }
    return false;
}
