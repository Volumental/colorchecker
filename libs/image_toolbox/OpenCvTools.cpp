#include "OpenCvTools.hpp"

#include "Color.hpp"
#include "Convert.hpp"

namespace komb {

Imageb maskFromCvMat(const cv::Mat& cv_image_in)
{
    cv::Mat cv_image = cv_image_in;

    // TODO(Emil): simplify and speed up.
    CHECK_EQ_F(cv_image.depth(), CV_8U);

    if (cv_image.channels() == 1)
    {
        return imageMaskFromCv(cv::Mat1b(cv_image));
    }

    if (cv_image.channels() == 4)
    {
        cv_image = ignoreAlpha(cv_image);
    }

    CHECK_EQ_F(cv_image.channels(), 3);

    return mapImage(makeConstArrayPointer(cv::Mat3b(cv_image)), [](const cv::Vec3b& bgr)
    {
        const float b = bgr[0] / 255.0f;
        const float g = bgr[1] / 255.0f;
        const float r = bgr[2] / 255.0f;
        return rgbIntensity({r, g, b}) > 0.5f;
    });
}

Imageb imageMaskFromCv(const cv::Mat1b& cv_mask)
{
    // TODO(mabur): make this work:  return mapImage(cv_mask, [](uint8_t c) { return c != 0; });
    Imageb out_mask(static_cast<size_t>(cv_mask.cols), static_cast<size_t>(cv_mask.rows));
    for (int y = 0; y < cv_mask.rows; ++y)
    {
        for (int x = 0; x < cv_mask.cols; ++x)
        {
            out_mask(static_cast<size_t>(x), static_cast<size_t>(y)) = cv_mask(y, x) > 127;
        }
    }
    return out_mask;
}

cv::Mat1b cvMaskFromImage(const Imageb& image_mask)
{
    const auto byte_from_bool = [](bool x) -> uint8_t { return x ? 255 : 0; };
    return makeCvMat(image_mask, byte_from_bool);
}

} // namespace komb
