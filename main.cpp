#include <iostream>
#include <Eigen/Core>
#include <Eigen/Geometry>

using namespace Eigen;

int main() {
    Matrix4f cameraToWorld;
    cameraToWorld <<
        0.718762, 0.615033, -0.324214, 0, 
        -0.393732, 0.744416, 0.539277, 0, 
        0.573024, -0.259959, 0.777216, 0, 
        0.526967, 1.254234, -2.53215, 1;
    
    Matrix4f worldToCamera = cameraToWorld.inverse();
    Vector4f PWorld(-0.315792, 1.4489, -2.48901, 1);
    Vector4f PCamera =  PWorld.transpose() * worldToCamera;

    std::cout << "P in camera coords = " << std::endl << PCamera << std::endl;
    return 0;
}
