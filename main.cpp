#include <iostream>
#include <fstream>
#include <vector>

#include <glm/vec3.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>

std::vector<glm::vec4> vertices = { 
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

const std::vector<glm::uint> triangles = { 
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
	const glm::vec4& worldCoordinate,
    const glm::mat4& worldToCamera,
    const float& bottom,
    const float& left,
    const float& top,
    const float& right,
    const float& near,
    const glm::uint& imageWidth,
    const glm::uint& imageHeight,
    glm::ivec2& pixelCoordinate)
{
    auto cameraCoordinate = worldToCamera * worldCoordinate;
    glm::vec2 screenCoordinate{ 
        cameraCoordinate.x / -(cameraCoordinate.z) * near,
        cameraCoordinate.y / -(cameraCoordinate.z) * near
    };

    glm::vec2 ndcCoordinate{
        (screenCoordinate.x + right) / (2 * right),
        (screenCoordinate.y + top) / (2 * top)
    };

    pixelCoordinate.x = static_cast<int>(ndcCoordinate.x * imageWidth);
    pixelCoordinate.y = static_cast<int>(ndcCoordinate.y * imageHeight);

    auto visible = true;
    if (screenCoordinate.x < 1 || screenCoordinate.x > right ||
        screenCoordinate.y < bottom || screenCoordinate.y > top)
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

glm::uint imageWidth = 640;
glm::uint imageHeight = 480;

enum class FitResolutionGate { kFill = 0, kOverscan };
FitResolutionGate fitFilm = FitResolutionGate::kOverscan;

float calculateRightCanvasCoord() {
    return ((filmApertureWidth * inchToMillimeters / 2) / focalLength) * nearClippingPlane;
}

float calculateTopCanvasCoord() {
    return ((filmApertureHeight * inchToMillimeters / 2) / focalLength) * nearClippingPlane;
}

int main() {
    auto filmAspectRatio = filmApertureWidth / filmApertureHeight;
    auto deviceAspectRatio = imageWidth / static_cast<float>(imageHeight);

    auto xScale = 1.f;
    auto yScale = 1.f;

    switch (fitFilm) {
        case FitResolutionGate::kFill:
            if (filmAspectRatio > deviceAspectRatio)
            {
                xScale = deviceAspectRatio / filmAspectRatio;
            }
            else {
                yScale = filmAspectRatio / deviceAspectRatio;
            }
            break;
        case FitResolutionGate::kOverscan:
            if (filmAspectRatio > deviceAspectRatio) {
                yScale = filmAspectRatio / deviceAspectRatio;
            }
            else {
                xScale = deviceAspectRatio / filmAspectRatio;
            }
            break;
        default:
            break;
    }

    auto top = calculateTopCanvasCoord() * yScale;
    auto right = calculateRightCanvasCoord() * xScale;
    auto bottom = -top;
    auto left = -right;
    const auto PI = 3.14f;

    std::cout << "Screen window bottom-left, top-right coordinates (" << left << ", " << bottom
       << ") (" << right << ", " << top << ")" << std::endl;
    std::cout << "Film Aspect Ratio: " << filmAspectRatio << std::endl;
    std::cout << "Device Aspect Ratio: " << deviceAspectRatio << std::endl;
    std::cout << "Angle of view: " << 2 * atan((filmApertureWidth * inchToMillimeters / 2) / focalLength) * 180 / PI << std::endl;

    std::ofstream image;
    image.open("./pinhole.svg");
    image << "<svg version=\"1.1\" xmlns:xlink=\"http://www.w3.org/1999/xlink\" xmlns=\"http://www.w3.org/2000/svg\" width=\"" << imageWidth << "\" height=\"" << imageHeight << "\">" << std::endl;

    glm::mat4 cameraToWorld{ 
        -0.95424f, 0.f, 0.299041f, 0.f, 
         0.0861242f, 0.95763f, 0.274823f, 0.f, 
        -0.28637f, 0.288002f, -0.913809f, 0.f, 
        -3.734612f, 7.610426f, -14.152769f, 1.f
    };
    glm::mat4 worldToCamera = glm::inverse(cameraToWorld);
    auto canvasWidth = 2.f;
    auto canvasHeight = 2.f;
    auto numberOfTriangle = triangles.size() / 3;

    for (glm::uint i = 0; i < numberOfTriangle; ++i) {
        const glm::vec4& v0World = vertices[triangles[i * 3]];
        const glm::vec4& v1World = vertices[triangles[i * 3 + 1]];
        const glm::vec4& v2World = vertices[triangles[i * 3 + 2]];
        glm::ivec2 v0Raster, v1Raster, v2Raster;

        auto visible = true;
        visible &= computePixelCoordinates(v0World, worldToCamera, bottom, left, top, right, nearClippingPlane, imageWidth, imageHeight, v0Raster);
        visible &= computePixelCoordinates(v1World, worldToCamera, bottom, left, top, right, nearClippingPlane, imageWidth, imageHeight, v1Raster);
        visible &= computePixelCoordinates(v2World, worldToCamera, bottom, left, top, right, nearClippingPlane, imageWidth, imageHeight, v2Raster);

        auto red = visible ? 0 : 255;
        image << "<line x1=\"" << v0Raster.x << "\" y1=\"" << v0Raster.y << "\" x2=\"" << v1Raster.x << "\" y2=\"" << v1Raster.y << "\" style=\"stroke:rgb(" << red << ",0,0);stroke-width:1\" />\n"; 
        image << "<line x1=\"" << v1Raster.x << "\" y1=\"" << v1Raster.y << "\" x2=\"" << v2Raster.x << "\" y2=\"" << v2Raster.y << "\" style=\"stroke:rgb(" << red << ",0,0);stroke-width:1\" />\n"; 
        image << "<line x1=\"" << v2Raster.x << "\" y1=\"" << v2Raster.y << "\" x2=\"" << v0Raster.x << "\" y2=\"" << v0Raster.y << "\" style=\"stroke:rgb(" << red << ",0,0);stroke-width:1\" />\n";
    }

    image << "</svg>\n";
    image.close();

    return 0;
}
