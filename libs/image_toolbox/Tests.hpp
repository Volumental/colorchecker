#pragma once

#include <opencv2/core/mat.hpp>

namespace komb {

/// Tests matrices for equal size, type and content.
bool areEqual(const cv::Mat& a, const cv::Mat& b);

bool isConstant(const cv::Mat& image);

inline bool isInsideImage(const cv::Point& p, const cv::Mat& image)
{
    return p.x >= 0 && p.y >= 0 && p.x < image.cols && p.y < image.rows;
}

}  // namespace komb
