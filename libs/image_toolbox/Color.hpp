#pragma once

#include <vector>

#include <Eigen/Core>
#include <opencv2/core/core.hpp>

#include <common/Random.hpp>
#include <geometry_toolbox/Types.hpp>

namespace komb {

struct Rgb
{
    float r; // [0 - 1]
    float g; // [0 - 1]
    float b; // [0 - 1]
};

struct Hsv
{
    float h; // [0 - 1]
    float s; // [0 - 1]
    float v; // [0 - 1]
};

/// See http://www.hsluv.org
struct HSLuv
{
    float h; // [0 - 1]
    float s; // [0 - 1]
    float l; // [0 - 1]. 0=black, 1=white
};

struct YCbCr
{
    float y;  // [0 - 1]
    float cb; // [0 - 1]
    float cr; // [0 - 1]
};

Rgb rgbFromHsv(const Hsv& in);
Rgb rgbFromHSLuv(const HSLuv& in); // NOLINT: camelCase function names
Rgb rgbFromYCbCr(const YCbCr& in); // NOLINT: camelCase function names

float rgbIntensity(const Rgb& rgb);

Hsv randomHsv(RandomEngine& io_rand);

Rgb randomBrightRgb(RandomEngine& io_rand);

std::vector<Rgb> distinctBrightColors(size_t count);

inline Vector3f rgbFromRgba(const Vector4f& c)
{
    return c.w() * Vector3f(c.x(), c.y(), c.z());
}

inline Vector4f rgbaFromRgb(const Vector3f& c)
{
    return Vector4f(c.x(), c.y(), c.z(), 1.0f);
}

VectorOfVector3f asEigenRgb(const cv::Mat& cv_colors);

} // namespace komb
