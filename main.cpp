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

vec3 random_in_unit_sphere() {
    vec3 p;
    do {
        p = 2.f*vec3(drand48(), drand48(), drand48()) - vec3(1.f, 1.f, 1.f);
    } while(p.squared_length() >= 1.f);
    return p;
}

vec3 color(const ray& r, const std::unique_ptr<hitable>& world) {
    hit_record rec;
    if (world->hit(r, 0.001f, std::numeric_limits<float>::max(), rec)) {
        vec3 target = rec.p + rec.normal + random_in_unit_sphere();
        return 0.5f*color( ray(rec.p, target - rec.p), world );
    }
    else {
        vec3 unit_direction = unit_vector(r.direction);
        float t = 0.5f*(unit_direction.y + 1.f);
        return (1.f - t) * vec3(1.f, 1.f, 1.f) + t*vec3(0.5f, 0.7f, 1.f);
    }
}

int main() {

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
                float u = (i + drand48()) / static_cast<float>(nx);
                float v = (j + drand48()) / static_cast<float>(ny);
                ray r = cam.get_ray(u, v);
                final_color += color(r, world);
            }
            final_color /= static_cast<float>(ns);
            final_color = vec3( sqrt(final_color.x), sqrt(final_color.y), sqrt(final_color.z) );
            int ir = static_cast<int>(255.99 * final_color.r);
            int ig = static_cast<int>(255.99 * final_color.g);
            int ib = static_cast<int>(255.99 * final_color.b);

            image << ir << " " << ig << " " << ib << std::endl;
        }
    }

    image.close();

    return 0;
}
