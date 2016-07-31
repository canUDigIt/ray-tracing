#include <iostream>

#include "vec3.h"

int main() {
    int nx(200);
    int ny(100);
    std::cout << "P3\n" << nx << " " << ny << "\n255" << std::endl;
    for (int j = ny-1; j >= 0; --j) {
        for (int i = 0; i < nx; ++i) {
            vec3 col(
                static_cast<float>(i) / static_cast<float>(nx),
                static_cast<float>(j) / static_cast<float>(ny),
                0.2f
            );
            int ir(255.99 * col.r);
            int ig(255.99 * col.g);
            int ib(255.99 * col.b);
            std::cout << ir << " " << ig << " " << ib << std::endl;
        }
    }
    return 0;
}