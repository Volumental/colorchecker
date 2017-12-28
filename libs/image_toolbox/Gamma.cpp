#include "Gamma.hpp"

#include <vector>

#include <opencv2/opencv.hpp>

#include <common/algorithm/Range.hpp>
#include <common/Math.hpp>

namespace komb {

cv::Mat1b byteFromLinear1(const cv::Mat1f& input)
{
    cv::Mat1b output(input.size());
    for (int ri : irange<int>(input.rows))
    {
        const auto p_in = input.ptr<float>(ri);
        auto p_out = output.ptr<uint8_t>(ri);
        for (int ci : irange<int>(input.cols))
        {
            p_out[ci] = srgbByteFromLinear(p_in[ci]);
        }
    }
    return output;
}

cv::Mat3b byteFromLinear3(const cv::Mat3f& input)
{
    cv::Mat3b output(input.size());
    for (int ri : irange<int>(input.rows))
    {
        const auto p_in = input.ptr<cv::Vec3f>(ri);
        auto p_out = output.ptr<cv::Vec3b>(ri);
        for (int ci : irange<int>(input.cols))
        {
            p_out[ci][0] = srgbByteFromLinear(p_in[ci][0]);
            p_out[ci][1] = srgbByteFromLinear(p_in[ci][1]);
            p_out[ci][2] = srgbByteFromLinear(p_in[ci][2]);
        }
    }
    return output;
}

cv::Mat4b byteFromLinear4(const cv::Mat4f& input)
{
    cv::Mat4b output(input.size());
    for (int ri : irange<int>(input.rows))
    {
        const auto p_in = input.ptr<cv::Vec4f>(ri);
        auto p_out = output.ptr<cv::Vec4b>(ri);
        for (int ci : irange<int>(input.cols))
        {
            p_out[ci][0] = srgbByteFromLinear(p_in[ci][0]);
            p_out[ci][1] = srgbByteFromLinear(p_in[ci][1]);
            p_out[ci][2] = srgbByteFromLinear(p_in[ci][2]);
            p_out[ci][3] = cv::saturate_cast<uint8_t>(p_in[ci][3] * 255.f); // Linear!
        }
    }
    return output;
}

cv::Mat1f linearFromByte1(const cv::Mat1b& input)
{
    cv::Mat1f output(input.size());
    for (int ri : irange<int>(input.rows))
    {
        const auto p_in = input.ptr<uint8_t>(ri);
        auto p_out = output.ptr<float>(ri);
        for (int ci : irange<int>(input.cols))
        {
            p_out[ci] = linearFromSrgbByte(p_in[ci]);
        }
    }
    return output;
}

cv::Mat3f linearFromByte3(const cv::Mat3b& input)
{
    cv::Mat3f output(input.size());
    for (int ri : irange<int>(input.rows))
    {
        const auto p_in = input.ptr<cv::Vec3b>(ri);
        auto p_out = output.ptr<cv::Vec3f>(ri);
        for (int ci : irange<int>(input.cols))
        {
            p_out[ci][0] = linearFromSrgbByte(p_in[ci][0]);
            p_out[ci][1] = linearFromSrgbByte(p_in[ci][1]);
            p_out[ci][2] = linearFromSrgbByte(p_in[ci][2]);
        }
    }
    return output;
}

cv::Mat4f linearFromByte4(const cv::Mat4b& input)
{
    cv::Mat4b output(input.size());
    for (int ri : irange<int>(input.rows))
    {
        const auto p_in = input.ptr<cv::Vec4b>(ri);
        auto p_out = output.ptr<cv::Vec4f>(ri);
        for (int ci : irange<int>(input.cols))
        {
            p_out[ci][0] = linearFromSrgbByte(p_in[ci][0]);
            p_out[ci][1] = linearFromSrgbByte(p_in[ci][1]);
            p_out[ci][2] = linearFromSrgbByte(p_in[ci][2]);
            p_out[ci][3] = static_cast<float>(p_in[ci][3]) / 255.0f; // Linear!
        }
    }
    return output;
}

}  // namespace komb
