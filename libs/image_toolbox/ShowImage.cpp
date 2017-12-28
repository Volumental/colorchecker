#include "ShowImage.hpp"

#include <string>

#include <opencv2/opencv.hpp>

#include <common/Units.hpp>

#include "Visualization.hpp"

namespace komb {

const int kWindowHSpacing = 5;
const int kWindowVSpacing = 25;
const bool kShadeDepthImages = true;

void imshowGrid(const std::string& title, const cv::Mat& mat, size_t row, size_t col)
{
    cv::imshow(title, mat);
    int x = kWindowHSpacing + (mat.cols + kWindowHSpacing) * static_cast<int>(col);
    int y = kWindowVSpacing + (mat.rows + kWindowVSpacing) * static_cast<int>(row);
    cv::moveWindow(title, x, y);
}

void makeOpenCvBendToMyCommand()
{
    // http://stackoverflow.com/a/25794701
    cv::waitKey(1);
    cv::waitKey(1);
    cv::waitKey(1);
    cv::waitKey(1);
}

/// Like cv::destroyWindow, but works.
void destroyWindow(const std::string& title)
{
    cv::destroyWindow(title);
    makeOpenCvBendToMyCommand();
}

/// Like cv::destroyAllWindows, but works.
void destroyAllWindows()
{
    cv::destroyAllWindows();
    makeOpenCvBendToMyCommand();
}

void imshowDepth(const std::string& title, const cv::Mat1f& depth_image)
{
    if (kShadeDepthImages)
    {
        imshow(title, cv::Mat1b(255 * shadeDepthImage(depth_image)));
    }
    else
    {
        imshow(title, cv::Mat1b(depth_image * 255.f));
    }
}

} // namespace komb
