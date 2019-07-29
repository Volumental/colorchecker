#pragma once

#include <opencv2/core.hpp>

namespace komb {

std::vector<cv::Point2f> simplifyContour(
    const std::vector<cv::Point>& contour, cv::Mat3b& canvas);

/**
 * @brief Detect and find colors of the squares in a colorchecker camera calibration target.
 * @param image
 * @param canvas Optional image where debug information will be drawn.
 * @return Colors of the colorchecker camera calibration target or empty if not found.
 */
cv::Mat3b findColorChecker(
    const cv::Mat3b& image, cv::Mat3b& canvas);

cv::Mat3b bigChecker(const cv::Mat3b& checker);

/**
 * @brief Fit a linear transformation from camera colors to reference colors.
 * @param camera_checker
 * @param reference_checker
 * @return 3x4 color transformation matrix.
 */
cv::Matx34f findColorTransformation(
    const cv::Mat3b& camera_checker,
    const cv::Mat3b& reference_checker);

/**
 * @brief Transform colors in-place.
 *
 * Treats colors as vectors in R3 and applies a rotation and translation on them
 * according to the given 3x4 transformation matrix.
 *
 * Consider using findColorTransformation() to find a transformation matrix that maps the colors
 * in one image to the colors in another image.
 *
 * @param image
 * @param color_transformation
 */
void applyColorTransformation(
    cv::Mat3b& image, const cv::Matx34f& color_transformation);

float medianAbsoluteDeviation(const cv::Mat& a, const cv::Mat& b);

} // namespace komb
