#pragma once

#include <string>

#include <opencv2/core.hpp>
#include <opencv2/highgui/highgui.hpp>

#include <common/algorithm/Container.hpp>
#include <common/ArrayPointer.hpp>
#include <image_toolbox/Image.hpp>
#include <image_toolbox/OpenCvTools.hpp>

namespace komb {

template<typename T>
inline void imshow(const std::string& title, const cv::Mat_<T>& image)
{
    cv::imshow(title, image);
}

template<>
inline void imshow<bool>(const std::string& title, const cv::Mat_<bool>& image)
{
    cv::imshow(title, image != false);
}

template<typename T>
inline void imshow(const std::string& title, ArrayPointer<const T, 2> image)
{
    const auto image_cv = makeCvMatConstPointer(image);
    komb::imshow(title, image_cv);
}

template<typename T>
inline void imshow(const std::string& title, const Image<T>& image)
{
    const auto image_pointer = ArrayPointer<const T, 2>(image);
    komb::imshow(title, image_pointer);
}

template<typename T>
inline void imshow(const std::string& title, const T* data, size_t width, size_t height)
{
    const auto image_pointer = ArrayPointer<const T, 2>(data, width, height);
    komb::imshow(title, image_pointer);
}

inline void imshowNormalized(const std::string& title, const cv::Mat& image)
{
    auto imagesc = image.clone();
    cv::normalize(imagesc, imagesc, image.depth() == CV_8U ? 255.0 : 1.0, 0.0, cv::NORM_MINMAX);
    cv::imshow(title, imagesc);
}

template<typename T>
inline void imshowNormalized(const std::string& title, ArrayPointer<const T, 2> image)
{
    const auto image_cv = makeCvMatConstPointer(image);
    komb::imshowNormalized(title, image_cv);
}

template<typename T>
inline void imshowNormalized(const std::string& title, const Image<T>& image)
{
    const auto image_pointer = ArrayPointer<const T, 2>(image);
    komb::imshowNormalized(title, image_pointer);
}

template<typename T>
inline void imshowNormalized(const std::string& title, const T* data, size_t width, size_t height)
{
    const auto image_pointer = ArrayPointer<const T, 2>(data, width, height);
    komb::imshowNormalized(title, image_pointer);
}

void imshowDepth(const std::string& title, const cv::Mat1f& depth_image);

/// Show a bunch of same-sized images in a grid, one at the time.
void imshowGrid(const std::string& title, const cv::Mat& mat, size_t row, size_t col);

/// Like cv::destroyWindow, but works.
void destroyWindow(const std::string& title);

/// Like cv::destroyAllWindows, but works.
void destroyAllWindows();

} // namespace komb
