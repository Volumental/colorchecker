#pragma once

#include "Image.hpp"
#include "OpenCvTools.hpp"

namespace komb {
namespace image {

/// Is the given coordinate within the image bounds?
template<typename Image>
bool contains(const Image& img, size_t x, size_t y)
{
    return x < width(img) && y < height(img);
}

/// Is the given coordinate within the image bounds?
template<typename Image>
bool contains(const Image& img, int x, int y)
{
    return 0 <= x && 0 <= y && contains(img, static_cast<size_t>(x), static_cast<size_t>(y));
}

} // namespace image
} // namespace komb
