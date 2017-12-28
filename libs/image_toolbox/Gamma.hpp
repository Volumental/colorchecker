#pragma once

#include <opencv2/core.hpp>

#include <geometry_toolbox/Types.hpp>

namespace komb {

/// Convert linear RGBA in [0, 1] to sRGBA [0, 255]
cv::Mat1b byteFromLinear1(const cv::Mat1f& channel);
cv::Mat3b byteFromLinear3(const cv::Mat3f& rgb);
cv::Mat4b byteFromLinear4(const cv::Mat4f& rgba);

/// Convert sRGBA in [0, 255] to linear [0, 1]
cv::Mat1f linearFromByte1(const cv::Mat1b& channel);
cv::Mat3f linearFromByte3(const cv::Mat3b& rgb);
cv::Mat4f linearFromByte4(const cv::Mat4b& rgba);

/// Convert sRGBA in [0, 1] to linear in [0, 1]
inline constexpr float linearFromSrgb(float s)
{
    return
        s <  0.f      ? -linearFromSrgb(-s) :
        s <= 0.04045f ? s / 12.92f      :
        std::pow((s + 0.055f) / 1.055f, 2.4f);
}

// TODO(Rasmus): Implement with lookup table.
/// Convert sRGBA in [0, 255] to linear in [0, 1]
inline constexpr float linearFromSrgbByte(uint8_t s)
{
    return linearFromSrgb(static_cast<float>(s) / 255.0f);
}

/// Convert linear in [0, 1] to sRGBA in [0, 1]
inline constexpr float srgbFromLinear(float s)
{
    return
        s < 0.f         ? -srgbFromLinear(-s) :
        s <= 0.0031308f ? 12.92f * s      :
        1.055f * std::pow(s, 0.41666f) - 0.055f;
}

/// Convert linear in [0, 1] to sRGBA in [0, 255]
inline constexpr uint8_t srgbByteFromLinear(float l)
{
    const float s = 255.0f * srgbFromLinear(l) + 0.5f;
    if (s <= 0) { return 0; }
    if (s >= 255.0f) { return 255; }
    return static_cast<uint8_t>(s);
}

/// Linear RGB from sRGB BGR
inline Vector3f linearFromCvVec3b(const cv::Vec3b& bgr)
{
    return Vector3f{
        linearFromSrgbByte(bgr[2]),
        linearFromSrgbByte(bgr[1]),
        linearFromSrgbByte(bgr[0]),
    };
}

} // namespace komb
