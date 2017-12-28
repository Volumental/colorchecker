#include "Convert.hpp"

#include <algorithm>
#include <vector>

#include <opencv2/opencv.hpp>

#include <common/Logging.hpp>

namespace komb {

std::vector<cv::Mat1b> rgb2Cmyk(const cv::Mat3b& img)
{
    // Adapted from https://gist.github.com/wyudong/9c392578c6247e7d1d28
    std::vector<cv::Mat1b> ret;
    // Allocate CMYK to store 4 components
    for (int i = 0; i < 4; ++i)
    {
        ret.push_back(cv::Mat(img.size(), CV_8UC1));
    }

    std::vector<cv::Mat> rgb;
    cv::split(img, rgb);

    for (int i = 0; i < img.rows; ++i)
    {
        for (int j = 0; j < img.cols; ++j)
        {
            float r = rgb[2].at<uchar>(i, j) / 255.0f;
            float g = rgb[1].at<uchar>(i, j) / 255.0f;
            float b = rgb[0].at<uchar>(i, j) / 255.0f;
            float k = std::min(std::min(1 - r, 1 - g), 1 - b);

            ret[0].at<uchar>(i, j) = (1 - r - k) / (1 - k) * 255.0f;
            ret[1].at<uchar>(i, j) = (1 - g - k) / (1 - k) * 255.0f;
            ret[2].at<uchar>(i, j) = (1 - b - k) / (1 - k) * 255.0f;
            ret[3].at<uchar>(i, j) = k * 255.0f;
        }
    }

    return ret;
}

cv::Mat convertAndRescale(const cv::Mat& src)
{
    cv::Mat out;
    convertAndRescale(src, out);
    return out;
}

// Only to 8 bit.
void convertAndRescale(const cv::Mat& src, cv::Mat& out_image)
{
    double minVal, maxVal;
    minMaxLoc(src, &minVal, &maxVal);
    src.convertTo(out_image, CV_8U, 255.0 / (maxVal - minVal), -minVal * 255.0 / (maxVal - minVal));
}

cv::Mat rescale(const cv::Mat& src)
{
    cv::Mat dst;
    double dst_max = src.depth() == CV_8U ? 255.0 : 1.0;
    cv::normalize(src, dst, 0, dst_max, cv::NORM_MINMAX);
    return dst;
}

cv::Mat toFloatMat(const cv::Mat& mat)
{
    if (mat.depth() == CV_32F)
    {
        return mat;
    }
    cv::Mat image;
    if (mat.depth() == CV_8U)
    {
        mat.convertTo(image, CV_32F, 1.0 / 255.0);
    }
    else
    {
        mat.convertTo(image, CV_32F);
    }
    return image;
}

cv::Mat3b ignoreAlpha(const cv::Mat4b& img)
{
    cv::Mat1b channels[4];
    cv::split(img, channels);

    cv::Mat3b out;
    cv::merge(channels, 3, out);

    return out;
}

} // namespace komb
