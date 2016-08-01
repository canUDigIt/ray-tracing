#include <iostream>
#include <fstream>
#include <limits>
#include <vector>
#include <memory>

#include "hitable_list.h"
#include "sphere.h"

vec3 color(const ray& r, const std::unique_ptr<hitable>& world) {
    hit_record rec;
    const vec3& center = vec3(0.f, 0.f, -1.f);
    if (world->hit(r, 0.f, std::numeric_limits<float>::max(), rec)) {
        return 0.5f*vec3(rec.normal.x+1, rec.normal.y+1, rec.normal.z+1);
    }
    else {
        vec3 unit_direction = unit_vector(r.direction);
        float t = 0.5f*(unit_direction.y + 1.f);
        return (1.0 - t) * vec3(1.f, 1.f, 1.f) + t*vec3(0.5f, 0.7f, 1.0);
    }
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

    std::vector< std::unique_ptr<hitable> > list(2);
    list[0].reset( new sphere(vec3(0.f, 0.f, -1.f), 0.5f) );
    list[1].reset( new sphere(vec3(0.f, -100.5f, -1.f), 100) );
    std::unique_ptr<hitable> world( new hitable_list(list.data(), list.size()) );

    for (int j = ny-1; j >= 0; --j) {
        for (int i = 0; i < nx; ++i) {
            float u = static_cast<float>(i) / static_cast<float>(nx);
            float v = static_cast<float>(j) / static_cast<float>(ny);
            ray r(origin, lower_left_corner + u*horizontal + v*vertical);
            vec3 col = color(r, world);
            int ir = static_cast<int>(255.99 * col.r);
            int ig = static_cast<int>(255.99 * col.g);
            int ib = static_cast<int>(255.99 * col.b);

            image << ir << " " << ig << " " << ib << std::endl;
        }
    }

    image.close();

    return 0;
}