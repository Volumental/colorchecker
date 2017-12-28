#include "Visualization.hpp"

#include <algorithm>
#include <string>
#include <vector>

#include <opencv2/opencv.hpp>

#include <common/algorithm/Range.hpp>
#include <common/algorithm/Vector.hpp>
#include <common/Math.hpp>
#include <geometry_toolbox/Angle.hpp>
#include <geometry_toolbox/Types.hpp>

#include "Color.hpp"
#include "Convert.hpp"
#include "Gamma.hpp"
#include "OpenCvTools.hpp"

namespace komb {

// Constants for sub-pixel drawing with opencv.
static const int kShift = 8;
static const float kShiftMultiplier = 1 << kShift;

Vector3f applyColormapScalar(float value, const VectorOfVector3f& colors)
{
    CHECK(!colors.empty());
    const size_t max_index = colors.size() - 1;
    const float t = value * static_cast<float>(max_index);
    const size_t a = std::min(floorToSize(t), max_index);
    const size_t b = std::min(ceilToSize(t), max_index);
    return colors[a] + (colors[b] - colors[a]) * (t - a);
}

cv::Mat3b applyColormap(const cv::Mat1f& image, const std::vector<cv::Scalar>& colors,
    float min_value, float max_value)
{
    CHECK(!colors.empty());
    const float max_i = static_cast<float>(colors.size()) - 1;
    return map(image, [&](float value)
    {
        const float t = remapClamp(value, min_value, max_value, 0.f, max_i);
        const size_t a = floorToSize(t);
        const size_t b = ceilToSize(t);
        const cv::Scalar color = colors[a] + (colors[b] - colors[a]) * (t - a);
        return cv::Vec3b(color[0], color[1], color[2]);
    });
}

void quiver(cv::Mat& io_canvas, const cv::Mat2f& flow, int spacing)
{
    if (io_canvas.size() != flow.size())
    {
        io_canvas.create(flow.size(), CV_8UC3);
        io_canvas.setTo(0);
    }

    // Rotation matrix for the arrowheads
    float cs = -0.3f * 0.86602540378f;
    float ss = -0.3f * 0.5f;
    for (int y = 0; y < flow.rows; y += spacing)
    {
        for (int x = 0; x < flow.cols; x += spacing)
        {
            cv::Vec2f delta = flow(y, x);
            double length = cv::norm(delta);
            if (length > 0)
            {
                cv::Point p0(x, y);
                cv::Point p1(x + delta[0], y + delta[1]);
                cv::Point p2(
                    p1.x + cs * delta[0] - ss * delta[1], p1.y + ss * delta[0] + cs * delta[1]);
                cv::Point p3(
                    p1.x + cs * delta[0] + ss * delta[1], p1.y - ss * delta[0] + cs * delta[1]);

                cv::line(io_canvas, p0, p1, cv::Scalar(0, 255, 0));
                cv::line(io_canvas, p1, p2, cv::Scalar(0, 255, 0));
                cv::line(io_canvas, p1, p3, cv::Scalar(0, 255, 0));
            }
        }
    }
}

cv::Mat1f shadeDepthImage(const cv::Mat1f& depth_image)
{
    // cv::Vec3f light_direction(-0.5, -0.5, -1);
    cv::Vec3f light_direction(0, 0, -1);
    light_direction = cv::normalize(light_direction);

    cv::Mat1f dx;
    cv::Mat1f dy;
    cv::Mat1f dz = depth_image / depth_image.cols;
    cv::Sobel(depth_image, dx, CV_32F, 1, 0);
    cv::Sobel(depth_image, dy, CV_32F, 0, 1);
    cv::Mat1f inv_norms = 1 / (dx.mul(dx) + dy.mul(dy) + dz.mul(dz));
    cv::sqrt(inv_norms, inv_norms);

    // Compute diffuse lighting.
    cv::Mat1f shaded = (dx * light_direction[0] + dy * light_direction[1] +
                                 -dz * light_direction[2]).mul(inv_norms);

    // Remove negative light.
    shaded.setTo(0, shaded < 0);

    // Add some ambient light to contrast against missing depth.
    shaded = shaded * 0.9f + 0.1f;
    shaded.setTo(0, depth_image == 0);

    // Gamma encode for viewing and storing.
    transform(shaded, shaded, srgbFromLinear);
    return shaded;
}

cv::Mat3f colorizeDepthImage(const cv::Mat1f& depth_image, Normalize normalize)
{
    // Use inverse depth for higher precision up close.
    cv::Mat1f inverse_depth = 1.f / depth_image;

    cv::Mat1b depth_char;
    double minval = 1.0 / 0.2;
    double maxval = 1.0 / 0.8;

    if (normalize == Normalize::kYes)
    {
        cv::minMaxLoc(inverse_depth, &minval, &maxval, nullptr, nullptr, depth_image != 0);
        // Equalize inverse depth.
        cv::Mat1b scaled_inverse_depth(255.f * (inverse_depth - minval) / (maxval - minval));
        cv::equalizeHist(scaled_inverse_depth, depth_char);
    }
    else
    {
        depth_char = cv::Mat1b(255.f * (inverse_depth - minval) / (maxval - minval));
    }

    // Colormapped depth.
    cv::Mat colormapped;
    cv::applyColorMap(255 - depth_char, colormapped, cv::COLORMAP_RAINBOW);
    colormapped.setTo(0, depth_image == 0);
    return cv::Mat3f(colormapped) * (1.0f / 255.0f);
}

cv::Mat3f colorizeAndShadeDepthImage(const cv::Mat1f& depth_image, Normalize normalize)
{
    // Colored depth.
    cv::Mat3f colormapped = komb::colorizeDepthImage(depth_image, normalize);

    // Shaded depth.
    cv::Mat1f shaded = komb::shadeDepthImage(depth_image);

    // Shaded colored depth.
    cv::Mat shaded_colormapped;
    cv::Mat multi_channel_shaded;
    cv::merge(std::vector<cv::Mat>({shaded, shaded, shaded}), multi_channel_shaded);
    cv::multiply(0.2f + multi_channel_shaded * 0.8f, colormapped, shaded_colormapped);
    return shaded_colormapped;
}

cv::Mat3f colorizeNormalMap(const cv::Mat3f& normals)
{
    std::vector<cv::Mat1f> n;
    cv::split(normals, n);
    cv::Mat3f colorized;
    cv::merge(std::vector<cv::Mat>{0.5f - n[2] * 0.5f, 0.5f - n[1] * 0.5f, 0.5f + n[0] * 0.5f},
        colorized);
    return colorized;
}

namespace cv_color {

const auto kRed    = cv::Scalar(0,     0, 255);
const auto kSilver = cv::Scalar(191, 191, 191);
const auto kBlue   = cv::Scalar(255,   0,   0);

} // namespace cv_color

cv::Mat3b colorizeAbsoluteErrors(const cv::Mat1f& errors, float max_error)
{
    CHECK(std::isfinite(max_error));
    CHECK_GT(max_error, 0.f);
    using namespace cv_color;
    return applyColormap(errors, {kSilver, kRed}, 0.f, max_error);
}

cv::Mat3b colorizeSignedErrors(const cv::Mat1f& errors, float max_error)
{
    CHECK(std::isfinite(max_error));
    CHECK_GT(max_error, 0.f);
    using namespace cv_color;
    return applyColormap(errors, {kRed, kSilver, kBlue}, -max_error, max_error);
}

cv::Mat3b encodeNormalMap(const cv::Mat3f& normals)
{
    return cv::Mat3b(colorizeNormalMap(normals) * 255);
}

cv::Mat3f decodeNormalMap(const cv::Mat3b& normals)
{
    std::vector<cv::Mat1b> n;
    cv::split(normals, n);
    const float f = 2.f / 255.f;
    std::vector<cv::Mat1f> decoded_components{cv::Mat1f(n[2]) * f - 1.f,
        1.f - cv::Mat1f(n[1]) * f, 1.f - cv::Mat1f(n[0]) * f};
    cv::Mat3f decoded;
    cv::merge(decoded_components, decoded);
    return decoded;
}

cv::Mat3b visualizeGradients(ArrayPointer<const float, 2> dx, ArrayPointer<const float, 2> dy)
{
    CHECK_EQ(dx.sizes(), dy.sizes());
    const auto width = image::width(dx);
    const auto height = image::height(dx);

    float max_grad = 0;
    for (const auto i : indices(dx))
    {
        max_grad = std::max(max_grad, std::hypot(dx[i], dy[i]));
    }

    cv::Mat3b gradient_img(height, width);
    for (const auto y : irange(height))
    {
        for (const auto x : irange(width))
        {
            const float strength = std::hypot(dx(x, y), dy(x, y)) / max_grad;
            const float angle = wrapZeroToTwoPi(std::atan2(dy(x, y), dx(x, y)));
            const auto rgb = rgbFromHsv({angle / kTau_f, std::sqrt(strength), strength});
            gradient_img(y, x) = cv::Vec3b(255 * rgb.b, 255 * rgb.g, 255 * rgb.r);
        }
    }
    return gradient_img;
}

cv::Mat plotMask(const cv::Mat& img, const ArrayPointer<const bool, 2>& mask_ptr)
{
    cv::Mat1b mask = makeCvMatConstPointer(mask_ptr).clone();
    CHECK_EQ(img.size(), mask.size());
    std::vector<std::vector<cv::Point>> contours;
    cv::findContours(mask.clone(), contours, cv::RETR_LIST, cv::CHAIN_APPROX_SIMPLE);
    cv::Mat canvas = img * 0.5;
    img.copyTo(canvas, mask);
    cv::drawContours(canvas, contours, -1, cv::Scalar(255, 255, 255));
    return canvas;
}

cv::Mat plotGrabCutMask(const cv::Mat& img, const cv::Mat& mask)
{
    CHECK_EQ(img.size(), mask.size());
    cv::Mat canvas = img.clone();
    cv::Mat(img * 0.5).copyTo(canvas, mask == cv::GC_BGD);
    cv::Mat(img * 0.3 + cv::Scalar(0, 0, 150)).copyTo(canvas, mask == cv::GC_PR_BGD);
    cv::Mat(img * 0.3 + cv::Scalar(0, 100, 150)).copyTo(canvas, mask == cv::GC_PR_FGD);
    cv::Mat(img * 1.0).copyTo(canvas, mask == cv::GC_FGD);

    cv::Mat all_fg_mask = (mask & cv::GC_FGD) != 0;
    cv::Mat fg_mask = mask == cv::GC_FGD;
    cv::Mat bg_mask = mask == cv::GC_BGD;
    std::vector<std::vector<cv::Point>> all_fg_contours;
    std::vector<std::vector<cv::Point>> fg_contours;
    std::vector<std::vector<cv::Point>> bg_contours;
    cv::findContours(all_fg_mask, all_fg_contours, cv::RETR_LIST, cv::CHAIN_APPROX_SIMPLE);
    cv::findContours(fg_mask, fg_contours, cv::RETR_LIST, cv::CHAIN_APPROX_SIMPLE);
    cv::findContours(bg_mask, bg_contours, cv::RETR_LIST, cv::CHAIN_APPROX_SIMPLE);
    cv::drawContours(canvas, bg_contours, -1, cv::Scalar(100, 100, 100));
    cv::drawContours(canvas, fg_contours, -1, cv::Scalar(150, 150, 150));
    cv::drawContours(canvas, all_fg_contours, -1, cv::Scalar(255, 255, 255));
    return canvas;
}

cv::Mat3b plotLabels(const cv::Mat_<int32_t>& labels)
{
    const auto num_labels = static_cast<size_t>(pickLargest(labels) + 1);
    return plotLabels(labels, num_labels);
}

cv::Mat3b plotLabels(const cv::Mat_<int32_t>& labels, size_t num_labels)
{
    CHECK_GT(num_labels, 0);

    // Randomize label colors
    unsigned int r = 14;
    std::vector<cv::Vec3b> label_colors(num_labels);
    label_colors[0] = cv::Vec3b(0, 0, 0);
    for (size_t i = 1; i < num_labels; ++i)
    {
        label_colors[i] = cv::Vec3b(rand_r(&r) & 255, rand_r(&r) & 255, rand_r(&r) & 255);
    }

    auto color_chooser = [&](int32_t label) -> cv::Vec3b
    {
        CHECK_GE(label, 0);
        CHECK_LT(static_cast<size_t>(label), num_labels);
        return label_colors[static_cast<size_t>(label)];
    };

    // Paint labeled areas in img
    return map(labels, color_chooser);
}

void scatter2(cv::Mat4b& io_canvas, const cv::Mat1f& points,
    const cv::Scalar& color, const cv::Mat& mask)
{
    // TODO(Rasmus): mask is used as cv::Mat_<bool> below without checking if it is of that type.
    // TODO(Rasmus): coordinates to cv::circle are clamped; they don't need to be clamped.
    // TODO(Rasmus): it would make sense to use subpixel precision when drawing circles.
    CHECK_EQ(points.cols, 2);
    CHECK(mask.empty() || mask.total() == static_cast<size_t>(points.rows));

    for (auto point_idx : irange(points.rows))
    {
        if (mask.empty() || mask.at<bool>(point_idx))
        {
            auto x = clamp(static_cast<int>(points(point_idx, 0)), 0, io_canvas.cols - 1);
            auto y = clamp(static_cast<int>(points(point_idx, 1)), 0, io_canvas.rows - 1);
            cv::circle(io_canvas, cv::Point(x, y), 2, color);
        }
    }
}

cv::Mat rotateCcw(const cv::Mat& canvas, float radians)
{
    cv::Point2f center = cv::Point2f(canvas.cols - 1, canvas.rows - 1) / 2.f;
    cv::Mat rot_mat = cv::getRotationMatrix2D(center, degreesFromRadians(radians), 1.0f);
    cv::Mat result;
    cv::warpAffine(canvas, result, rot_mat, canvas.size());
    return result;
}

cv::Mat rotateCcw90(const cv::Mat& image, int degrees)
{
    degrees = ((degrees % 360) + 360) % 360;
    if (degrees == 0)
    {
        return image;
    }

    cv::Mat out;
    if (degrees == 90)
    {
        cv::transpose(image, out);
        cv::flip(out, out, 0);
    }
    else if (degrees == 180)
    {
        cv::flip(image, out, -1);
    }
    else if (degrees == 270)
    {
        cv::transpose(image, out);
        cv::flip(out, out, 1);
    }
    else
    {
        ABORT_F("Invalid angle: %d", degrees);
    }
    return out;
}

void circleSubPix(
    cv::Mat& io_canvas, const cv::Point2f& center,
    float radius, const cv::Scalar& color, int thickness)
{
    cv::Point shifted_center(center.x * kShiftMultiplier, center.y * kShiftMultiplier);
    const int shifted_radius = roundToInt(radius * kShiftMultiplier);
    cv::circle(io_canvas, shifted_center, shifted_radius, color, thickness, CV_AA, kShift);
}

void arrowedLineSubpix(cv::Mat& io_canvas, const cv::Point2f& base, const cv::Point2f& tip,
    const cv::Scalar& color, int thickness, double tip_length)
{
    cv::arrowedLine(io_canvas, base * kShiftMultiplier, tip * kShiftMultiplier, color, thickness,
        CV_AA, kShift, tip_length);
}

std::vector<cv::Point> shiftPoints(const std::vector<cv::Point2f>& points)
{
    std::vector<cv::Point> shifted_points;
    shifted_points.reserve(points.size());
    for (auto& p : points)
    {
        cv::Point p_shifted(p.x * kShiftMultiplier, p.y * kShiftMultiplier);
        shifted_points.push_back(p_shifted);
    }
    return shifted_points;
}

void lineSubPix(cv::Mat& io_canvas, const cv::Point2f& a, const cv::Point2f& b,
    const cv::Scalar& color, int thickness)
{
    polyLinesSubPix(io_canvas, {a, b}, false, color, thickness);
}

void polyLinesSubPix(
    cv::Mat& io_canvas, const std::vector<cv::Point2f>& points,
    bool closed, const cv::Scalar& color, int thickness) // NOLINT (bool param)
{
    std::vector<cv::Point> shifted_points = shiftPoints(points);
    cv::polylines(io_canvas, shifted_points, closed, color, thickness, cv::LINE_AA, kShift);
}

void fillPolySubPix(
    cv::Mat& io_canvas, const std::vector<cv::Point2f>& points, const cv::Scalar& color)
{
    std::vector<std::vector<cv::Point>> shifted_points = { shiftPoints(points) };
    cv::fillPoly(io_canvas, shifted_points, color, cv::LINE_AA, kShift);
}

void putTextCentered(
    cv::Mat&           io_canvas,
    const std::string& text,
    const cv::Point&   center,
    int                font_face,
    double             font_scale,
    const              cv::Scalar& color,
    int                thickness,
    int                line_type)
{
    int baseLine = 0;
    cv::Size text_size = cv::getTextSize(text, font_face, font_scale, thickness, &baseLine);
    cv::Point origin(center.x - text_size.width / 2, center.y);
    cv::putText(io_canvas, text, origin, font_face, font_scale, color, thickness, line_type);
}

cv::Mat3b plotCovariance(const cv::Mat& covariance, bool sign_by_color) // NOLINT bool param
{
    cv::Mat intensity;
    cv::sqrt(cv::abs(covariance), intensity);
    double maxVal;
    cv::minMaxLoc(intensity, 0, &maxVal);
    cv::Mat1b scaled;

    if (sign_by_color)
    {
        cv::Mat(-intensity).copyTo(intensity, covariance < 0);
        cv::convertScaleAbs(intensity, scaled, 128.0 / std::max(maxVal, 0.1), 127);
    }
    else
    {
        cv::Mat1b tri(covariance.size(), 0);
        int m = covariance.rows - 1;
        cv::Point2i points[3] = {cv::Point2i(0, 1), cv::Point2i(0, m), cv::Point2i(m - 1, m)};
        cv::fillConvexPoly(tri, points, 3, cv::Scalar(255));
        intensity.setTo(0, tri & (covariance > 0));
        intensity.setTo(0, tri.t() & (covariance < 0));
        cv::convertScaleAbs(intensity, scaled, 255.0 / std::max(maxVal, 0.1), 0);
    }

    cv::Mat3b colored;
    cv::applyColorMap(scaled, colored, cv::COLORMAP_JET);
    return colored;
}

cv::Mat1b plotCovarianceGaussian(const cv::Mat1d& covariance, size_t i, size_t j)
{
    // Square root of covariance matrix, can be used for plotting uncertainties
    cv::Matx22d Cov(covariance(i, i), covariance(i, j),
        covariance(j, i), covariance(j, j));
    cv::Matx21d E;
    cv::Matx22d V;
    cv::eigen(Cov, E, V);
    cv::Matx21d sqrtE;
    cv::sqrt(E, sqrtE);
    cv::Matx22d sqrt_cov = V * cv::Matx22d::diag(sqrtE) * V.t();

    cv::Mat1b img(500, 500);
    double s = 4.0 * sqrt(cv::max(Cov(0, 0), Cov(1, 1))) / img.cols;
    cv::Matx22d T = sqrt_cov.inv() * s;
    for (int r = 0; r < img.rows; ++r)
    {
        for (int c = 0; c < img.cols; ++c)
        {
            cv::Vec2d p(c - img.cols / 2, img.rows / 2 - r);
            double v = cv::norm(T * p);
            img(r, c) = cv::saturate_cast<uint8_t>((2.0 - v) * 255.0);
        }
    }
    return img;
}

cv::Mat3b applyGamma(const cv::Mat3b& image_in, float gamma)
{
    if (gamma == 1.0f) { return image_in; }

    auto image_out = image_in.clone();

    const std::vector<uint8_t> gamma_lookup = mapVector(irange(255), [gamma](int i) -> uint8_t
    {
        return roundToByte(std::pow(i / 255.0f, 1.0f / gamma) * 255.0f);
    });

    for (cv::Vec3b& pixel : image_out)
    {
        for (const int i : irange(3))
        {
            pixel[i] = gamma_lookup[pixel[i]];
        }
    }

    return image_out;
}

}  // namespace komb
