#pragma once

#include <opencv2/core/mat.hpp>

namespace komb {

double norm(const cv::Point2f& p);

double distance(const cv::Point2f& a, const cv::Point2f& b);

double squaredNorm(const cv::Point2f& p);

int squaredNorm(const cv::Point2i& p);

double squaredDistance(const cv::Point2f& a, const cv::Point2f& b);

int squaredDistance(const cv::Point2i& a, const cv::Point2i& b);

void normalize(cv::Point2f& io_p);

cv::Point2f normalized(const cv::Point2f& p);

} // namespace komb
