#include "MetricOpenCv.hpp"

#include <opencv2/opencv.hpp>

namespace komb {

double norm(const cv::Point2f& p)
{
    return std::sqrt(p.ddot(p));
}

double distance(const cv::Point2f& a, const cv::Point2f& b)
{
    return norm(b - a);
}

double squaredNorm(const cv::Point2f& p)
{
    return p.ddot(p);
}

double squaredDistance(const cv::Point2f& a, const cv::Point2f& b)
{
    return squaredNorm(b - a);
}

int squaredNorm(const cv::Point2i& p)
{
    return p.dot(p);
}

int squaredDistance(const cv::Point2i& a, const cv::Point2i& b)
{
    return squaredNorm(b - a);
}

void normalize(cv::Point2f& io_p)
{
    if (io_p != cv::Point2f(0, 0))
    {
        io_p /= norm(io_p);
    }
}

cv::Point2f normalized(const cv::Point2f& p)
{
    if (p == cv::Point2f(0, 0))
    {
        return p;
    }
    return p / norm(p);
}

} // namespace komb
