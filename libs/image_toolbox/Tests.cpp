#include "Tests.hpp"

#include <opencv2/opencv.hpp>

namespace komb {

bool areEqual(const cv::Mat& a, const cv::Mat& b)
{
    if (a.size() != b.size())
    {
        return false;
    }
    if (a.type() != b.type())
    {
        return false;
    }
    return cv::countNonZero(a.reshape(1) != b.reshape(1)) == 0;
}

bool isConstant(const cv::Mat& image)
{
    double min_val;
    double max_val;
    cv::minMaxLoc(image, &min_val, &max_val);
    return min_val == max_val;
}

}  // namespace komb
