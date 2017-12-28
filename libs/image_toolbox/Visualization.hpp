#pragma once

#include <vector>
#include <string>

#include <opencv2/core.hpp>

#include <common/ArrayPointer.hpp>
#include <common/Logging.hpp>
#include <geometry_toolbox/Types.hpp>

namespace komb {

enum class Normalize { kNo, kYes };

/// Convert values in the range 0 to 1 to the linear interpolation of the values in colors.
Vector3f applyColormapScalar(float value, const VectorOfVector3f& colors);
cv::Mat3b applyColormap(const cv::Mat1f& image, const std::vector<cv::Scalar>& colors,
    float min_value = 0.0f, float max_value = 1.0f);

void quiver(cv::Mat& io_canvas, const cv::Mat2f& flow, int spacing);
cv::Mat1f shadeDepthImage(const cv::Mat1f& depth_image);
cv::Mat3f colorizeDepthImage(const cv::Mat1f& depth_image, Normalize normalize = Normalize::kYes);
cv::Mat3f colorizeAndShadeDepthImage(
    const cv::Mat1f& depth_image, Normalize normalize = Normalize::kYes);
cv::Mat3f colorizeNormalMap(const cv::Mat3f& normals);
cv::Mat3b colorizeAbsoluteErrors(const cv::Mat1f& errors, float max_error);
cv::Mat3b colorizeSignedErrors(const cv::Mat1f& errors, float max_error);

/**
 * @brief Encode a normal map as a color image ready to be stored on disk.
 * @param normals
 * @return
 */
cv::Mat3b encodeNormalMap(const cv::Mat3f& normals);

/**
 * @brief Decode a normal map from a color image (probably from disk).
 * @param normals
 * @return
 */
cv::Mat3f decodeNormalMap(const cv::Mat3b& normals);

/// Given an image gradient, return a visualization of it.
cv::Mat3b visualizeGradients(ArrayPointer<const float, 2> dx, ArrayPointer<const float, 2> dy);

cv::Mat plotMask(const cv::Mat& img, const ArrayPointer<const bool, 2>& mask);
cv::Mat plotGrabCutMask(const cv::Mat& img, const cv::Mat& mask);
cv::Mat3b plotLabels(const cv::Mat_<int32_t>& labels);
cv::Mat3b plotLabels(const cv::Mat_<int32_t>& labels, size_t num_labels);

/// @param mask is be per-point
void scatter2(cv::Mat4b& io_canvas, const cv::Mat1f& points,
    const cv::Scalar& color, const cv::Mat& mask = cv::Mat());

/// Rotate an image counterclockwise any angle.
cv::Mat rotateCcw(const cv::Mat& canvas, float radians);

/// Rotate an image counterclockwise any 90 degrees step, eg 270, -90, etc.
cv::Mat rotateCcw90(const cv::Mat& canvas, int degrees);

/// Use thickness = CV_FILLED for filled circle.
void circleSubPix(
    cv::Mat& io_canvas, const cv::Point2f& center,
    float radius, const cv::Scalar& color, int thickness);

void lineSubPix(cv::Mat& io_canvas, const cv::Point2f& a, const cv::Point2f& b,
    const cv::Scalar& color, int thickness);

void arrowedLineSubpix(cv::Mat& io_canvas, const cv::Point2f& base, const cv::Point2f& tip,
    const cv::Scalar& color, int thickness = 1, double tip_length = 0.1);

void polyLinesSubPix(
    cv::Mat& io_canvas, const std::vector<cv::Point2f>& points,
    bool closed, const cv::Scalar& color, int thickness); // NOLINT (bool param)

void fillPolySubPix(
    cv::Mat& io_canvas, const std::vector<cv::Point2f>& points, const cv::Scalar& color);

void plotConicSection(
    cv::Mat& io_canvas,
    const cv::Mat_<float>& coefficients,
    const std::vector<cv::Point2f>& points,
    const cv::Scalar& color, int thickness);

void plotPolynomial(
    cv::Mat& io_canvas,
    const cv::Mat_<float>& coefficients, size_t num_points,
    const cv::Scalar& color, int thickness);

void putTextCentered(
    cv::Mat&           io_canvas,
    const std::string& text,
    const cv::Point&   center,
    int                font_face,
    double             font_scale,
    const              cv::Scalar& color,
    int                thickness = 1,
    int                line_type = cv::LINE_8);

cv::Mat3b plotCovariance(const cv::Mat& covariance, bool sign_by_color); // NOLINT bool parameter

cv::Mat1b plotCovarianceGaussian(const cv::Mat1d& covariance, size_t i, size_t j);

/// Higher gamma = brighter
cv::Mat3b applyGamma(const cv::Mat3b& image_in, float gamma);

} // namespace komb
