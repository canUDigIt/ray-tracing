#include <iostream>
#include <fstream>
#include <limits>
#include <vector>
#include <memory>
#include <ctime>
#include <random>

#include "hitable_list.h"
#include "sphere.h"
#include "camera.h"

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
    std::random_device random_device;
    std::mt19937 generator(random_device());
    std::uniform_real_distribution<float> distribution(0, 1);

    int nx(200);
    int ny(100);
    int ns(100);

    std::ofstream image("test.ppm", std::ios::trunc);
    image << "P3\n" << nx << " " << ny << "\n255" << std::endl;

    std::vector< std::unique_ptr<hitable> > list(2);
    list[0].reset( new sphere(vec3(0.f, 0.f, -1.f), 0.5f) );
    list[1].reset( new sphere(vec3(0.f, -100.5f, -1.f), 100) );
    std::unique_ptr<hitable> world( new hitable_list(list.data(), list.size()) );

    camera cam;

    for (int j = ny-1; j >= 0; --j) {
        for (int i = 0; i < nx; ++i) {
            vec3 final_color(0.f, 0.f, 0.f);
            for (int s = 0; s < ns; ++s) {
                float u = (i /*+ distribution(generator)*/) / static_cast<float>(nx);
                float v = (j /*+ distribution(generator)*/) / static_cast<float>(ny);
                ray r = cam.get_ray(u, v);
                final_color += color(r, world);
            }
            final_color /= static_cast<float>(ns);

            int ir = static_cast<int>(255.99 * final_color.r);
            int ig = static_cast<int>(255.99 * final_color.g);
            int ib = static_cast<int>(255.99 * final_color.b);

            image << ir << " " << ig << " " << ib << std::endl;
        }
    }

    image.close();

    return 0;
}