#include <iostream>
#include <fstream>
#include <cstdint>
#include <chrono>

#include <Eigen/Geometry>

#include "cow.h"

static constexpr auto inchToMillimeters = 25.4f;
enum class FitResolutionGate { kFill = 0, kOverscan };

void computeScreenCoordinates(
    float filmApertureWidth,
    float filmApertureHeight,
    uint32_t imageWidth,
    uint32_t imageHeight,
    FitResolutionGate fitFilm,
    float nearClippingPlane,
    float focalLength,
    float& top, float& bottom, float& left, float& right
)
{
    float filmAspectRatio = filmApertureWidth / filmApertureHeight;
    float deviceAspectRatio = imageWidth / static_cast<float>(imageHeight);

    top = ((filmApertureHeight * inchToMillimeters / 2 ) / focalLength) * nearClippingPlane;
    right = ((filmApertureWidth * inchToMillimeters / 2) / focalLength) * nearClippingPlane;

    // field of view (horizontal)
    auto fov = 2 * 180 / M_PI * atan((filmApertureWidth * inchToMillimeters / 2) / focalLength);
    std::cerr << "Field of view " << fov << std::endl;

    auto xScale = 1.f;
    auto yScale = 1.f;

    switch (fitFilm) {
        default:
        case FitResolutionGate::kFill:
            if (filmAspectRatio > deviceAspectRatio) {
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
    }

    right *= xScale;
    top *= yScale;

    bottom = -top;
    left = -right;
}

bool convertToRaster(
	const Eigen::Vector4f& vertexWorld,
    const Eigen::Matrix4f& worldToCamera,
    float bottom,
    float left,
    float top,
    float right,
    float near,
    uint32_t imageWidth,
    uint32_t imageHeight,
    Eigen::Vector4f& vertexRaster)
{
    auto cameraCoordinate = worldToCamera * vertexWorld;

    Eigen::Vector2f screenCoordinate{ 
        cameraCoordinate.x() / -cameraCoordinate.z() * near,
        cameraCoordinate.y() / -cameraCoordinate.z() * near
    };

    Eigen::Vector2f ndcCoordinate{
        2 * screenCoordinate.x() / (right - left) - (right + left) / (right - left),
        2 * screenCoordinate.y() / (top - bottom) - (top + bottom) / (top - bottom)
    };

    vertexRaster.x() = (ndcCoordinate.x() + 1) / 2 * imageWidth;
    vertexRaster.y() = (1 - ndcCoordinate.y()) / 2 * imageHeight;
}

float min3(float a, float b, float c) { 
    return std::min(a, std::min(b, c)); 
}

float max3(float a, float b, float c) {
    return std::max(a, std::max(b, c));
}

float edgeFunction(const Eigen::Vector4f& a, const Eigen::Vector4f& b, const Eigen::Vector4f& c) {
    return (c.x() - a.x()) * (b.y() - a.y()) - (c.y() - a.y()) * (b.x() - a.x());
}

float edgeFunctionCCW(const Eigen::Vector4f& a, const Eigen::Vector4f& b, const Eigen::Vector4f& c) {
    return (a.x() - b.x()) * (c.y() - a.y()) - (a.y() - b.y()) * (c.x() - a.x());
}

constexpr auto imageWidth = 640;
constexpr auto imageHeight = 480;
Eigen::Matrix4f worldToCamera;

const auto numberOfTriangles = 3156;
auto nearClippingPlane = 1.f;
auto farClippingPlane = 1000.f;
auto focalLength = 20.f; // in mm
// 35mm Full Aperture in inches
auto filmApertureWidth = 0.980f;
auto filmApertureHeight = 0.735f;

using Rgb = std::array<unsigned char, 3>;

int main() {
    // Make the data for this matrix colomn...right now I'm setting the columns
    // as rows. Should be transposed.
    worldToCamera << 
        0.707107, -0.331295, 0.624695, 0, 
        0, 0.883452, 0.468521, 0, 
        -0.707107, -0.331295, 0.624695, 0, 
        -1.63871, -5.74777, -40.400412, 1;
    worldToCamera.transposeInPlace();
    
    Eigen::Matrix4f cameraToWorld = worldToCamera.inverse();

    float t, b, l, r;

    computeScreenCoordinates(
        filmApertureWidth,
        filmApertureHeight,
        imageWidth,
        imageHeight,
        FitResolutionGate::kOverscan, 
        nearClippingPlane,
        focalLength,
        t, b, l, r);
    
    std::array<Rgb, imageWidth * imageHeight> framebuffer;
    framebuffer.fill({255,255,255});

    std::array<float, imageWidth * imageHeight> depthbuffer;
    depthbuffer.fill(farClippingPlane);

    auto startTime = std::chrono::high_resolution_clock::now();

    for (auto i = 0; i < numberOfTriangles; ++i) {
        Eigen::Vector4f v0 = Eigen::Vector4f::UnitW();
        Eigen::Vector4f v1 = Eigen::Vector4f::UnitW();
        Eigen::Vector4f v2 = Eigen::Vector4f::UnitW();
        v0.block<3,1>(0,0) = vertices[nvertices[i * 3]].cast<float>();
        v1.block<3,1>(0,0) = vertices[nvertices[i * 3 + 1]].cast<float>();
        v2.block<3,1>(0,0) = vertices[nvertices[i * 3 + 2]].cast<float>();

        Eigen::Vector4f v0Raster, v1Raster, v2Raster;

        convertToRaster(v0, worldToCamera, l, r, t, b, nearClippingPlane, imageWidth, imageHeight, v0Raster);
        convertToRaster(v1, worldToCamera, l, r, t, b, nearClippingPlane, imageWidth, imageHeight, v1Raster);
        convertToRaster(v2, worldToCamera, l, r, t, b, nearClippingPlane, imageWidth, imageHeight, v2Raster);

        v0Raster.z() = 1 / v0Raster.z();
        v1Raster.z() = 1 / v1Raster.z();
        v2Raster.z() = 1 / v2Raster.z();

        Eigen::Vector2f st0 = st[stindices[i * 3]].cast<float>();
        Eigen::Vector2f st1 = st[stindices[i * 3 + 1]].cast<float>();
        Eigen::Vector2f st2 = st[stindices[i * 3 + 2]].cast<float>();

        st0 *= v0Raster.z(), st1 *= v1Raster.z(), st2 *= v2Raster.z();

        auto xmin = min3(v0Raster.x(), v1Raster.x(), v2Raster.x());
        auto ymin = min3(v0Raster.y(), v1Raster.y(), v2Raster.y());
        auto xmax = max3(v0Raster.x(), v1Raster.x(), v2Raster.x());
        auto ymax = max3(v0Raster.y(), v1Raster.y(), v2Raster.y());

        if (xmin > imageWidth - 1 || xmax < 0 || ymin > imageHeight - 1 || ymax < 0)
            continue;

        auto x0 = std::max(0, static_cast<int32_t>(std::floor(xmin)));
        auto x1 = std::min(imageWidth - 1, static_cast<int32_t>(std::floor(xmax)));
        auto y0 = std::max(0, static_cast<int32_t>(std::floor(ymin)));
        auto y1 = std::min(imageHeight - 1, static_cast<int32_t>(std::floor(ymax)));

        auto area = edgeFunction(v0, v1, v2);

        for (uint32_t y = y0; y <= y1; ++y) {
            for (uint32_t x = x0; x <= x1; ++x) {
                Eigen::Vector4f pixelSample = {x + 0.5f, y + 0.5f, 0.f, 1.f};
                auto w0 = edgeFunction(v1Raster, v2Raster, pixelSample);
                auto w1 = edgeFunction(v2Raster, v0Raster, pixelSample);
                auto w2 = edgeFunction(v0Raster, v1Raster, pixelSample);
                if (w0 >= 0 && w1 >= 0 && w2 >= 0) {
                    w0 /= area;
                    w1 /= area;
                    w2 /= area;
                    auto oneOverZ = v0Raster.z() * w0 + v1Raster.z() * w1 + v2Raster.z() * w2;
                    auto z = 1 / oneOverZ;

                    if (z < depthbuffer[y * imageWidth + x]) {
                        depthbuffer[y * imageWidth + x] = z;

                        Eigen::Vector2f st = st0 * w0 + st1 * w1 + st2 * w2;

                        st *= z;

                        Eigen::Vector4f v0Cam = worldToCamera * v0;
                        Eigen::Vector4f v1Cam = worldToCamera * v1;
                        Eigen::Vector4f v2Cam = worldToCamera * v2;

                        auto px = (v0Cam.x() / -v0Cam.z()) * w0 + (v1Cam.x() / -v1Cam.z()) * w1 + (v2Cam.x() / -v2Cam.z()) * w2;
                        auto py = (v0Cam.y() / -v0Cam.z()) * w0 + (v1Cam.y() / -v1Cam.z()) * w1 + (v2Cam.y() / -v2Cam.z()) * w2;
                        Eigen::Vector4f pt{px * z, py * z, -z, 0.f};

                        Eigen::Vector4f a = (v1Cam - v0Cam);
                        Eigen::Vector4f b = (v2Cam - v0Cam);
                        Eigen::Vector4f n = a.cross3(b);
                        n.normalize();
                        Eigen::Vector4f viewDirection = -pt;
                        viewDirection.normalize();

                        auto nDotView = std::max(0.f, n.dot(viewDirection));

                        const auto M = 10;
                        auto checker = (fmod(st.x() * M, 1.f) > 0.5f) ^ (fmod(st.y() + M, 1.f) < 0.5f);
                        auto c = 0.3f * (1.f - checker) + 0.7f * checker;
                        nDotView *= c;
                        framebuffer[y * imageWidth + x][0] = nDotView * 255;
                        framebuffer[y * imageWidth + x][1] = nDotView * 255;
                        framebuffer[y * imageWidth + x][2] = nDotView * 255;
                    }
                }
            }
        }
    }

    auto endTime = std::chrono::high_resolution_clock::now();
    auto passedTime = std::chrono::duration<double, std::milli>(endTime - startTime).count();
    std::cerr << "Wall passed time: " << passedTime << " ms" << std::endl;

    std::ofstream imageFile;
    imageFile.open("./output.ppm");
    imageFile << "P6\n" << imageWidth << " " << imageHeight << "\n255\n";
    imageFile.write(reinterpret_cast<char *>(framebuffer.front().data()), imageWidth * imageHeight * 3);
    imageFile.close();

    return 0;
}
