#include <iostream>
#include <fstream>
#include <limits>
#include <vector>
#include <memory>

#include "hitable_list.h"
#include "sphere.h"
#include "camera.h"
#include "random_numbers.h"
#include "math_constants.h"

template<typename T>
std::unique_ptr<T> make_unique(T* ptr) {
    return std::unique_ptr<T>(ptr);
}

hitable* random_scene() {
    int n = 500;
    std::vector< std::unique_ptr<hitable> > list(n+1);
    list[0] = make_unique( new sphere(vec3(0, -1000, 0), 1000, std::unique_ptr<material>(new lambertian(vec3(0.5, 0.5, 0.5)))) );
    int i = 1;
    for (int a = -11; a < 11; a++) {
        for (int b = -11; b < 11; b++) {
                float choose_mat = uniform_random<float>();
                vec3 center(a+0.9*uniform_random<float>(), 0.2, b+0.9*uniform_random<float>());
                if ((center - vec3(4, 0.2, 0)).length() > 0.9) {
                    if (choose_mat < 0.8) { // diffuse
                        list[i++] = make_unique( new sphere(center, 0.2, std::unique_ptr<material>( new lambertian(vec3(uniform_random<float>()*uniform_random<float>(), uniform_random<float>()*uniform_random<float>(), uniform_random<float>()*uniform_random<float>())))) ); 
                    }
                    else if (choose_mat < 0.95) {
                        list[i++] = make_unique( new sphere(center, 0.2, std::unique_ptr<material>( new metal(vec3(0.5*(1+uniform_random<float>()), 0.5*(1+uniform_random<float>()), 0.5*(1+uniform_random<float>())), 0.5*uniform_random<float>()))) );
                    }
                    else { // glass
                        list[i++] = make_unique( new sphere(center, 0.2, std::unique_ptr<material>(new dielectric(1.5))) );
                    }
                }
        }
    }
    list[i++] = make_unique( new sphere(vec3(0, 1, 0), 1.0, std::unique_ptr<material>(new dielectric(1.5))) );
    list[i++] = make_unique( new sphere(vec3(-4, 1, 0), 1.0, std::unique_ptr<material>(new lambertian(vec3(04, 0.2, 0.1)))) );
    list[i++] = make_unique( new sphere(vec3(4, 1, 0), 1.0, std::unique_ptr<material>(new metal(vec3(0.7, 0.6, 0.5), 0.0))) );

    return new hitable_list(list.data(), i);
}

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
    std::unique_ptr<material> lambertian2( new lambertian(vec3(0.8f, 0.8f, 0.f)) );
    std::unique_ptr<material> metal1( new metal(vec3(0.8f, 0.6f, 0.2f), 0.3f) );
    std::unique_ptr<material> dielectric1( new dielectric(1.5f) );
    std::unique_ptr<material> dielectric2( new dielectric(1.5f) );

    float R = cos(pi/4);
    list[0].reset( new sphere( vec3(0.f, 0.f, -1.f), 0.5f, std::move(lambertian1) ) );
    list[1].reset( new sphere( vec3(0.f, 100.5f, -1.f), 100.f, std::move(lambertian2) ) );
    list[2].reset( new sphere( vec3(1.f, 0.f, -1.f), 0.5f, std::move(metal1) ) );
    list[3].reset( new sphere( vec3(-1.f, 0.f, -1.f), 0.5f, std::move(dielectric1) ) );
    list[4].reset( new sphere( vec3(-1.f, 0.f, -1.f), -0.45, std::move(dielectric2) ) );
    
    std::unique_ptr<hitable> world( random_scene() );

    vec3 lookfrom(3,3,2);
    vec3 lookat(0,0,-1);
    float dist_to_focus = (lookfrom-lookat).length();
    float aperture = 2.0;

    camera cam(lookfrom, lookat, vec3(0, 1, 0), 20, static_cast<float>(nx) / static_cast<float>(ny), aperture, dist_to_focus);

    for (int j = ny-1; j >= 0; --j) {
        for (int i = 0; i < nx; ++i) {
            vec3 final_color(0.f, 0.f, 0.f);
            for (int s = 0; s < ns; ++s) {
                float u = (i + uniform_random<float>()) / static_cast<float>(nx);
                float v = (j + uniform_random<float>()) / static_cast<float>(ny);
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
