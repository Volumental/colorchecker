#pragma once

#include <vector>

#include <opencv2/core.hpp>

namespace komb {

/**
 * @brief Generalized gradient magnitude over all channels.
 *
 * Computes the root of the sum of squared x and y derivatices of all channels.
 *
 * @param image
 * @return Edge magnitude for each pixel.
 */
cv::Mat1f edgeMagnitude(const cv::Mat& image);

/**
 * @brief Generalization of cv::magnitude
 *
 * Treats each image as a component of a vector and takes the magnitude of that
 * vector (L2-norm), pixel by pixel. The images must all have the same size.
 *
 * Eg. You have an image with x, an image with y and an image with z. You want
 * to compute the magnitude of each vector so you call
 *
 *     cv::Mat1f mag = magnitude(std::vector<cv::Mat>{x, y, z});
 */
cv::Mat1f magnitude(const std::vector<cv::Mat>& images);

} // namespace komb
