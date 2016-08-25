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

vec3 color(const ray& r, const std::unique_ptr<hitable>& world, int depth) {
    hit_record rec;
    if (world->hit(r, 0.001f, std::numeric_limits<float>::max(), rec)) {
        ray scattered;
        vec3 attenuation;
        if (depth < 50 && rec.mat_ptr->scatter(r, rec, attenuation, scattered)) {
            return attenuation*color(scattered, world, depth + 1);
        }
        else {
            return vec3(0.f, 0.f, 0.f);
        }
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

    std::vector< std::unique_ptr<hitable> > list(5);

    std::unique_ptr<material> lambertian1( new lambertian(vec3(0.1f, 0.2f, 0.5f)) );
    std::unique_ptr<material> lambertian2( new lambertian(vec3(0.8f, 0.8f, 0.0f)) );
    std::unique_ptr<material> metal1( new metal(vec3(0.8f, 0.6f, 0.2f), 0.3f) );
    std::unique_ptr<material> dielectric1( new dielectric(1.5f) );
    std::unique_ptr<material> dielectric2( new dielectric(1.5f) );

    list[0].reset( new sphere( vec3(0.f, 0.f, -1.f), 0.5f, std::move(lambertian1) ) );
    list[1].reset( new sphere( vec3(0.f, -100.5f, -1.f), 100.f, std::move(lambertian2) ) );
    list[2].reset( new sphere( vec3(1.f, 0.f, -1.f), 0.5f, std::move(metal1) ) );
    list[3].reset( new sphere( vec3(-1.f, 0.f, -1.f), 0.5f, std::move(dielectric1) ) );
    list[4].reset( new sphere( vec3(-1.f, 0.f, -1.f), -0.45, std::move(dielectric2) ) );
    
    std::unique_ptr<hitable> world( new hitable_list(list.data(), list.size()) );

    camera cam;

    for (int j = ny-1; j >= 0; --j) {
        for (int i = 0; i < nx; ++i) {
            vec3 final_color(0.f, 0.f, 0.f);
            for (int s = 0; s < ns; ++s) {
                float u = (i + drand48()) / static_cast<float>(nx);
                float v = (j + drand48()) / static_cast<float>(ny);
                ray r = cam.get_ray(u, v);
                final_color += color(r, world, 0);
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
