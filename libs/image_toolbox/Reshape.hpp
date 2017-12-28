#pragma once

#include <algorithm>
#include <cmath>

#include <opencv2/core.hpp>
#include <vector>

#include <common/Logging.hpp>

namespace komb {

/**
 * @brief Reshape the tiling of images.
 *
 * The typical use case is to tile many small images in a square so they are
 * easier to display.
 *
 * From this:
 *
 *     ┌───┐
 *     │ 1 │
 *     ├───┤
 *     │ 2 │
 *     ├───┤
 *     │ 3 │
 *     ├───┤
 *     │ 4 │
 *     └───┘
 *
 * To this:
 *
 *     ┌───┬───┐
 *     │ 1 │ 2 │
 *     ├───┼───┤
 *     │ 3 │ 4 │
 *     └───┴───┘
 *
 * But the reverse is also possible by setting the @a tiling_hint.
 *
 * Tiling is done left to right, up to down. Just like text.
 *
 * @param src
 * @param cell_width
 * @param cell_height
 * @param tiling_hint
 * * 0: Tile a square shape
 * * >0: Max cols for tiling
 * * <0: Max rows for tiling
 * @param border
 * @param color
 * @return
 */
cv::Mat reshapeTiles(
        const cv::Mat& src,
        int cell_width,
        int cell_height,
        int tiling_hint = 0,
        int border = 0,
        const cv::Scalar& color = cv::Scalar(0));

cv::Mat reshapeAndTile(
        const cv::Mat& src,
        int cell_width,
        int cell_height,
        int tiling_hint = 0,
        int border = 0,
        const cv::Scalar& color = cv::Scalar(0));

template <typename T>
void imageToVector(
        const cv::Mat_<T>& img,
        const cv::Mat1b& mask,
        std::vector<T>& vec)
{
    CHECK_EQ(img.size(), mask.size());
    vec.clear();

    // Loop through mask and insert selected pixels from img into vec
    for (int y = 0; y < img.rows; ++y)
    {
        for (int x = 0; x < img.cols; ++x)
        {
            if (mask(y, x))
            {
                vec.push_back(img(y, x));
            }
        }
    }
}

template <typename T>
void imageToVector(
    const cv::Mat_<T>& img,
    const cv::Mat1b&   mask,
    cv::Mat_<T>&       out_vec)
{
    CHECK_EQ(img.size(), mask.size());
    out_vec.create(1, cv::countNonZero(mask));

    // Loop through mask and insert selected pixels from img into out_vec
    size_t index = 0;
    for (int y = 0; y < img.rows; ++y)
    {
        for (int x = 0; x < img.cols; ++x)
        {
            if (mask(y, x))
            {
                out_vec(index) = img(y, x);
                ++index;
            }
        }
    }
}

template <typename T>
cv::Mat_<T> imageToVector(const cv::Mat_<T>& img, const cv::Mat1b& mask)
{
    cv::Mat_<T> vec;
    imageToVector<T>(img, mask, vec);
    return vec;
}

template <typename T>
void vectorToImage(
        const std::vector<T> &vec,
        const cv::Mat1b& mask,
        cv::Mat_<T>& img)
{
    CHECK_EQ(img.size(), mask.size());

    // Loop through mask and insert selected pixels from vec into img
    size_t counter = 0;
    for (int y = 0; y < img.rows; ++y)
    {
        for (int x = 0; x < img.cols; ++x)
        {
            if (mask(y, x))
            {
                img(y, x) = vec[counter];
                ++counter;
            }
        }
    }
    CHECK_EQ(counter, vec.size());
}

template <typename T>
void vectorToImage(const cv::Mat_<T>& vec, const cv::Mat1b& mask, cv::Mat_<T>& img)
{
    CHECK_EQ(img.size(), mask.size());
    CHECK(vec.rows == 1 || vec.cols == 1) << "vec has size " << vec.size()
            << " but should be a row or column vector.";

    // Loop through mask and insert selected pixels from vec into img.
    int counter = 0;
    for (int y = 0; y < img.rows; ++y)
    {
        for (int x = 0; x < img.cols; ++x)
        {
            if (mask(y, x))
            {
                img(y, x) = vec(counter);
                ++counter;
            }
        }
    }
    CHECK_EQ(counter, vec.size().area());
}

template <typename T>
cv::Mat_<T> vectorToImage(const cv::Mat_<T>& vec, const cv::Mat1b& mask)
{
    cv::Mat_<T> img(mask.size(), 0);
    vectorToImage<T>(vec, mask, img);
    return img;
}

template <typename T>
void maskToIndices(const cv::Mat1b& mask, std::vector<T>& indices)
{
    indices.clear();

    // Loop through mask and store indices of non-zero elements
    size_t i = 0;
    for (int y = 0; y < mask.rows; ++y)
    {
        for (int x = 0; x < mask.cols; ++x)
        {
            if (mask(y, x))
            {
                indices.push_back(i);
            }
            ++i;
        }
    }
}

template <typename T>
void indicesToMask(const std::vector<T>& indices, cv::Mat1b& out_mask)
{
    out_mask.setTo(0);

    // Loop through indices and mark them in the mask
    for (const auto i : indices)
    {
        out_mask(i) = 255;
    }
}

} // namespace komb
