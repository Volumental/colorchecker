#include "Magnitude.hpp"

#include <vector>

#include <opencv2/opencv.hpp>

#include <common/Logging.hpp>

namespace komb {

cv::Mat1f edgeMagnitude(const cv::Mat& image)
{
    std::vector<cv::Mat> channels;
    cv::split(image, channels);
    cv::Mat1f magnitude(image.size(), 0);
    cv::Mat dx;
    cv::Mat dy;
    cv::Mat square;
    for (const auto& channel : channels)
    {
        cv::Sobel(channel, dx, CV_32F, 1, 0, 3, 1. / 16);
        cv::multiply(dx, dx, square);
        magnitude += square;

        cv::Sobel(channel, dy, CV_32F, 0, 1, 3, 1. / 16);
        cv::multiply(dy, dy, square);
        magnitude += square;
    }
    cv::sqrt(magnitude, magnitude);
    return magnitude;
}

cv::Mat1f magnitude(const std::vector<cv::Mat>& images)
{
    CHECK(!images.empty());
    cv::Mat1f square_sum;
    cv::Mat1f img = images[0];
    cv::multiply(img, img, square_sum);
    for (size_t i = 1; i < images.size(); ++i)
    {
        CHECK_EQ(images[i].size(), images[0].size());
        img = images[i];
        cv::Mat1f square;
        cv::multiply(img, img, square);
        square_sum += square;
    }
    cv::Mat1f mag;
    cv::sqrt(square_sum, mag);
    return mag;
}

} // namespace komb
