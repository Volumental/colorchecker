#include "Color.hpp"

#include <vector>

#include <Eigen/Core>

#include <common/Logging.hpp>
#include <geometry_toolbox/Angle.hpp>

#include "Convert.hpp"

namespace komb {

Rgb rgbFromHsv(const Hsv& in)
{
    // This function is based on ImGui::ColorConvertHSVtoRGB.

    // gray
    if (in.s == 0.0f) { return Rgb{in.v, in.v, in.v}; }

    float h = fmodf(in.h, 1.0f) / (60.0f/360.0f);
    int   i = static_cast<int>(h);
    float f = h - static_cast<float>(i);
    float p = in.v * (1.0f - in.s);
    float q = in.v * (1.0f - in.s * f);
    float t = in.v * (1.0f - in.s * (1.0f - f));

    if (i == 0) { return Rgb{in.v, t,    p   }; }
    if (i == 1) { return Rgb{q,    in.v, p   }; }
    if (i == 2) { return Rgb{p,    in.v, t   }; }
    if (i == 3) { return Rgb{p,    q,    in.v}; }
    if (i == 4) { return Rgb{t,    p,    in.v}; }

    return Rgb{in.v, p, q};
}

Rgb rgbFromYCbCr(const YCbCr& in) // NOLINT: camelCase function names
{
    // Constants from https://docs.opencv.org/3.1.0/de/d25/imgproc_color_conversions.html
    const float delta = 0.5f;
    return {
        in.y + 1.403f * (in.cr - delta),
        in.y - 0.714f * (in.cr - delta) - 0.344f * (in.cb - delta),
        in.y + 1.773f * (in.cb - delta)
    };
}

float rgbIntensity(const Rgb& color)
{
    return 0.3f * color.r + 0.59f * color.g + 0.11f * color.b;
}

Hsv randomHsv(RandomEngine& io_rand)
{
    std::uniform_real_distribution<float> uniform_01(0, 1);

    // The HSV color space is a cone, with the pointy end at v=0 (black).
    // This is how we sample the cone uniformly:
    float hue        = uniform_01(io_rand);
    float saturation = std::sqrt(uniform_01(io_rand));
    float value      = std::cbrt(uniform_01(io_rand));
    return {hue, saturation, value};
}

bool isBrightAndSaturated(const Rgb& rgb, const Hsv& hsv)
{
    return rgbIntensity(rgb) > 0.3f && hsv.s > 0.2f;
}

Rgb randomBrightRgb(RandomEngine& io_rand)
{
    Hsv hsv;
    Rgb rgb;
    do
    {
        hsv = randomHsv(io_rand);
        rgb = rgbFromHsv(hsv);
    } while (!isBrightAndSaturated(rgb, hsv));
    return rgb;
}

std::vector<Rgb> distinctBrightColors(size_t count)
{
    const auto seed = std::random_device()();
    auto random_engine = getRandomEngine(seed);

    std::vector<Rgb> results;
    results.reserve(count);

    double hue = 0;
    const double kGoldenRatio = 0.61803398875;

    for (size_t i = 0; i < count; ++i)
    {
        Hsv hsv;
        Rgb rgb;

        do {
            hsv = randomHsv(random_engine);
            hsv.h = hue;
            rgb = rgbFromHsv(hsv);

            hue += kGoldenRatio;
            if (hue > 1) { hue -= 1; }
        } while (!isBrightAndSaturated(rgb, hsv));

        results.push_back(rgb);
    }

    return results;
}

VectorOfVector3f asEigenRgb(const cv::Mat& cv_colors)
{
    VectorOfVector3f result;

    if (cv_colors.channels() == 1)
    {
        for (const auto& value : cv::Mat1f(toFloatMat(cv_colors)))
        {
            result.emplace_back(value, value, value);
        }
    }
    else if (cv_colors.channels() == 3)
    {
        for (const auto& pixel : cv::Mat3f(toFloatMat(cv_colors)))
        {
            result.emplace_back(pixel[2], pixel[1], pixel[0]);
        }
    }
    else
    {
        ABORT_S() << "Expected 1 or 3 channels, got: " << cv_colors.channels();
    }

    return result;
}

} // namespace komb
