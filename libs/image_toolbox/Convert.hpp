#pragma once

#include <vector>

#include <opencv2/core/mat.hpp>

namespace komb {

std::vector<cv::Mat1b> rgb2Cmyk(const cv::Mat3b& img);

cv::Mat convertAndRescale(const cv::Mat& src);
void convertAndRescale(const cv::Mat& src, cv::Mat& out_image);
cv::Mat rescale(const cv::Mat& src);

cv::Mat toFloatMat(const cv::Mat& mat);

cv::Mat3b ignoreAlpha(const cv::Mat4b& img);

} // namespace komb
