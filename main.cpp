#include <iostream>
#include <fstream>
#include <vector>
#include <array>
#include <cstdint>

#include <Eigen/Core>
#include <Eigen/Geometry>

std::vector<Eigen::Vector4f> vertices = { 
    {  -2.5703,   0.78053,  -2.4e-05, 1}, { -0.89264,  0.022582,  0.018577, 1}, 
    {   1.6878, -0.017131,  0.022032, 1}, {   3.4659,  0.025667,  0.018577, 1}, 
    {  -2.5703,   0.78969, -0.001202, 1}, { -0.89264,   0.25121,   0.93573, 1}, 
    {   1.6878,   0.25121,    1.1097, 1}, {   3.5031,   0.25293,   0.93573, 1}, 
    {  -2.5703,    1.0558, -0.001347, 1}, { -0.89264,    1.0558,    1.0487, 1}, 
    {   1.6878,    1.0558,    1.2437, 1}, {   3.6342,    1.0527,    1.0487, 1}, 
    {  -2.5703,    1.0558,        0., 1}, { -0.89264,    1.0558,       0., 1}, 
    {   1.6878,    1.0558,        0., 1}, {   3.6342,    1.0527,       0., 1}, 
    {  -2.5703,    1.0558,  0.001347, 1}, { -0.89264,    1.0558,   -1.0487, 1}, 
    {   1.6878,    1.0558,   -1.2437, 1}, {   3.6342,    1.0527,   -1.0487, 1}, 
    {  -2.5703,   0.78969,  0.001202, 1}, { -0.89264,   0.25121,  -0.93573, 1}, 
    {   1.6878,   0.25121,   -1.1097, 1}, {   3.5031,   0.25293,  -0.93573, 1}, 
    {   3.5031,   0.25293,        0., 1}, {  -2.5703,   0.78969,       0., 1}, 
    {   1.1091,    1.2179,        0., 1}, {    1.145,     6.617,       0., 1}, 
    {   4.0878,    1.2383,        0., 1}, {  -2.5693,    1.1771, -0.081683, 1}, 
    {  0.98353,    6.4948, -0.081683, 1}, { -0.72112,    1.1364, -0.081683, 1}, 
    {   0.9297,     6.454,        0., 1}, {  -0.7929,     1.279,       0., 1}, 
    {  0.91176,    1.2994,        0., 1} 
}; 

const std::vector<uint32_t> triangles = { 
     4,   0,   5,   0,   1,   5,   1,   2,   5,   5,   2,   6,   3,   7,   2, 
     2,   7,   6,   5,   9,   4,   4,   9,   8,   5,   6,   9,   9,   6,  10, 
     7,  11,   6,   6,  11,  10,   9,  13,   8,   8,  13,  12,  10,  14,   9, 
     9,  14,  13,  10,  11,  14,  14,  11,  15,  17,  16,  13,  12,  13,  16, 
    13,  14,  17,  17,  14,  18,  15,  19,  14,  14,  19,  18,  16,  17,  20, 
    20,  17,  21,  18,  22,  17,  17,  22,  21,  18,  19,  22,  22,  19,  23, 
    20,  21,   0,  21,   1,   0,  22,   2,  21,  21,   2,   1,  22,  23,   2, 
     2,  23,   3,   3,  23,  24,   3,  24,   7,  24,  23,  15,  15,  23,  19, 
    24,  15,   7,   7,  15,  11,   0,  25,  20,   0,   4,  25,  20,  25,  16, 
    16,  25,  12,  25,   4,  12,  12,   4,   8,  26,  27,  28,  29,  30,  31, 
    32,  34,  33 
};

bool computePixelCoordinates(
	const Eigen::Vector4f& worldCoordinate,
    const Eigen::Matrix4f& worldToCamera,
    const float& bottom,
    const float& left,
    const float& top,
    const float& right,
    const float& near,
    const uint32_t& imageWidth,
    const uint32_t& imageHeight,
    Eigen::Vector2i& pixelCoordinate)
{
    auto cameraCoordinate = worldToCamera * worldCoordinate;

    Eigen::Vector2f screenCoordinate{ 
        cameraCoordinate.x() / -cameraCoordinate.z() * near,
        cameraCoordinate.y() / -cameraCoordinate.z() * near
    };

    Eigen::Vector2f ndcCoordinate{
        (screenCoordinate.x() + right) / (2 * right),
        (screenCoordinate.y() + top) / (2 * top)
    };

    pixelCoordinate.x() = static_cast<int>(ndcCoordinate.x() * imageWidth);
    pixelCoordinate.y() = static_cast<int>((1 - ndcCoordinate.y()) * imageHeight);

    auto visible = true;
    if (screenCoordinate.x() < 1 || screenCoordinate.x() > right ||
        screenCoordinate.y() < bottom || screenCoordinate.y() > top)
    {
        visible = false;
    }

    return visible;
}

auto focalLength = 35.f;
auto filmApertureWidth = 0.980f;
auto filmApertureHeight = 0.735f;

static const auto inchToMillimeters = 25.4f;

auto nearClippingPlane = 0.1f;
auto farClippingPlane = 1000.f;

uint32_t imageWidth = 640;
uint32_t imageHeight = 480;

enum class FitResolutionGate { kFill = 0, kOverscan };
FitResolutionGate fitFilm = FitResolutionGate::kOverscan;

float calculateRightCanvasCoord() {
    return ((filmApertureWidth * inchToMillimeters / 2.f) / focalLength) * nearClippingPlane;
}

float calculateTopCanvasCoord() {
    return ((filmApertureHeight * inchToMillimeters / 2.f) / focalLength) * nearClippingPlane;
}

float edgeFunction(const Eigen::Vector2f& a, const Eigen::Vector2f& b, const Eigen::Vector2f& c) {
    return (c.x() - a.x()) * (b.y() - a.y()) - (c.y() - a.y()) * (b.x() - a.x());
}

using Rgb = std::array<char, 3>;

int main() {
    Eigen::Vector2f v0  = {491.407, 411.407};
    Eigen::Vector2f v1 = {148.593, 68.5928};
    Eigen::Vector2f v2 = {148.593, 411.407};
    Eigen::Vector3f c0 = {1, 0, 0};
    Eigen::Vector3f c1 = {0, 1, 0};
    Eigen::Vector3f c2 = {0, 0, 1};

    const uint32_t w = 512;
    const uint32_t h = 512;

    std::array<Rgb, w * h> framebuffer;
    framebuffer.fill({0,0,0});

    auto area = edgeFunction(v0, v1, v2);

    for (uint32_t j = 0; j < h; ++j) {
        for (uint32_t i = 0; i < w; ++i) {
            Eigen::Vector2f p = {i + 0.5f, j + 0.5f};
            float w0 = edgeFunction(v1, v2, p);
            float w1 = edgeFunction(v2, v0, p);
            float w2 = edgeFunction(v0, v1, p);
            if (w0 >= 0 && w1 >= 0 && w2 >= 0) {
                w0 /= area;
                w1 /= area;
                w2 /= area;
                float r = w0 * c0.x() + w1 * c1.x() + w2 * c2.x();
                float g = w0 * c0.y() + w1 * c1.y() + w2 * c2.y();
                float b = w0 * c0.z() + w1 * c1.z() + w2 * c2.z();
                framebuffer[j * w + i][0] = static_cast<char>(r * 255);
                framebuffer[j * w + i][1] = static_cast<char>(g * 255);
                framebuffer[j * w + i][2] = static_cast<char>(b * 255);
            }
        }
    }

    std::ofstream imageFile;
    imageFile.open("./raster2d.ppm");
    imageFile << "P6\n" << w << " " << h << "\n255\n";
    imageFile.write(framebuffer.front().data(), w * h * 3);
    imageFile.close();

    return 0;
}
