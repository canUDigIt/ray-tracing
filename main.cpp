#include <iostream>
#include <fstream>

#include "ray.h"

vec3 color(const ray& r) {
    vec3 unit_direction = unit_vector(r.direction);
    float t = 0.5f*(unit_direction.y + 1.f);
    return (1.0 - t) * vec3(1.f, 1.f, 1.f) + t*vec3(0.5f, 0.7f, 1.0);
}

int main() {
    int nx(200);
    int ny(100);

    std::ofstream image("test.ppm", std::ios::trunc);
    image << "P3\n" << nx << " " << ny << "\n255" << std::endl;

    vec3 lower_left_corner(-2.f, -1.f, -1.f);
    vec3 horizontal(4.f, 0.f, 0.f);
    vec3 vertical(0.f, 2.f, 0.f);
    vec3 origin(0.f, 0.f, 0.f);

    for (int j = ny-1; j >= 0; --j) {
        for (int i = 0; i < nx; ++i) {
            float u = static_cast<float>(i) / static_cast<float>(nx);
            float v = static_cast<float>(j) / static_cast<float>(ny);
            ray r(origin, lower_left_corner + u*horizontal + v*vertical);
            vec3 col( color(r) );
            int ir = static_cast<int>(255.99 * col.r);
            int ig = static_cast<int>(255.99 * col.g);
            int ib = static_cast<int>(255.99 * col.b);

            image << ir << " " << ig << " " << ib << std::endl;
        }
    }

    image.close();

    return 0;
}