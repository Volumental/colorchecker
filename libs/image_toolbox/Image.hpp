#pragma once

#include <algorithm>
#include <array>
#include <type_traits>
#include <valarray>

#include <Eigen/Geometry>

#include <common/algorithm/Container.hpp>
#include <common/ArrayPointer.hpp>

namespace komb {

template<typename T>
class Image
{
public:
    using value_type = T;
    using iterator = T*;
    using const_iterator = const T*;

    Image() : width_(0), height_(0), pixels_(0) {}

    Image(size_t width_arg, size_t height_arg)
        : width_(width_arg), height_(height_arg), pixels_(width_arg * height_arg) {}

    Image(size_t width_arg, size_t height_arg, const T& value)
        : width_(width_arg), height_(height_arg), pixels_(value, width_arg * height_arg) {}

    Image(size_t width_arg, size_t height_arg, const T* value)
        : width_(width_arg), height_(height_arg), pixels_(value, width_arg * height_arg) {}

    explicit Image(const std::array<size_t, 2>& dimensions)
        : width_(dimensions[0]), height_(dimensions[1])
        , pixels_(dimensions[0] * dimensions[1]) {}

    Image(const std::array<size_t, 2>& dimensions, const T& value)
        : width_(dimensions[0]), height_(dimensions[1])
        , pixels_(value, dimensions[0] * dimensions[1]) {}

    explicit Image(const ArrayPointer<const T, 2>& array_pointer)
        : Image(array_pointer.size(0), array_pointer.size(1), array_pointer.data())
    {}

    T*       data()        {return std::begin(pixels_);}
    const T* data()  const {return std::begin(pixels_);}
    T*       begin()       {return std::begin(pixels_);}
    const T* begin() const {return std::begin(pixels_);}
    T*       end()         {return std::end(pixels_);}
    const T* end()   const {return std::end(pixels_);}

    size_t size()    const {return pixels_.size();}
    size_t width()   const {return width_;}
    size_t height()  const {return height_;}
    bool   empty()   const {return size() == 0;}

    T&       operator[](size_t i)       {return pixels_[i];}
    const T& operator[](size_t i) const {return pixels_[i];}

    T&       operator()(size_t x, size_t y)       { return pixels_[y * width_ + x]; }
    const T& operator()(size_t x, size_t y) const { return pixels_[y * width_ + x]; }

    void swap(Image<T>& io_image)
    {
        std::swap(width_,  io_image.width_);
        std::swap(height_, io_image.height_);
        std::swap(pixels_, io_image.pixels_);
    }

private:
    size_t width_;
    size_t height_;
    std::valarray<T> pixels_;
};

using Imageb = Image<bool>;
using Imagef = Image<float>;
using Imaged = Image<double>;
using Image3f = Image<Eigen::Vector3f>;

template<typename T>
void swap(Image<T>& a, Image<T>& b)
{
    a.swap(b);
}

// For implicit conversion to ArrayPointer.
template<typename T>
size_t multidimensionalSize(const Image<T>& image, size_t dimension)
{
    if (dimension == 0) { return image.width(); }
    if (dimension == 1) { return image.height(); }
    return 1;
}

namespace image {

template<typename T> size_t  width(const Image<T>& image) { return image.width();  }
template<typename T> size_t height(const Image<T>& image) { return image.height(); }
template<typename T> size_t   size(const Image<T>& image) { return image.size();   }
template<typename T> const T* data(const Image<T>& image) { return image.data();   }
template<typename T> T*       data(Image<T>& image)       { return image.data();   }

template<typename T>
std::array<size_t, 2> dimensions(const Image<T>& image)
{
    return {image.width(), image.height()};
}

} // namespace image

inline Imaged convertToDouble(const Imagef& image_float)
{
    auto image_double = Imaged(image_float.width(), image_float.height());
    cast(image_float, image_double);
    return image_double;
}

inline Imagef convertToFloat(const Imaged& image_double)
{
    auto image_float = Imagef(image_double.width(), image_double.height());
    cast(image_double, image_float);
    return image_float;
}

template <typename ImageIn>
auto makeImage(const ImageIn& in)
{
    const auto width  = image::width(in);
    const auto height = image::height(in);
    const auto data   = image::data(in);
    using value_type =
        typename std::remove_const<typename std::remove_reference<decltype(*data)>::type>::type;
    return Image<value_type>(width, height, data);
}

template <typename ImageIn, typename UnaryOperation>
auto mapImage(const ImageIn& in, UnaryOperation op)
{
    using value_type_in  = decltype(*in.begin());
    using value_type_out = decltype(op(value_type_in{}));

    const auto width  = image::width(in);
    const auto height = image::height(in);

    auto out = Image<value_type_out>(width, height);
    transform(in, out, op);
    return out;
}

template <typename ImageIn1, typename ImageIn2, typename BinaryOperation>
auto mapImage(const ImageIn1& in1, const ImageIn2& in2, BinaryOperation op)
{
    CHECK_EQ(image::dimensions(in1), image::dimensions(in2));
    using value_type_out = decltype(op(*in1.begin(), *in2.begin()));
    const auto width  = image::width(in1);
    const auto height = image::height(in1);
    auto out = Image<value_type_out>(width, height);
    transform(in1, in2, out, op);
    return out;
}

template <typename T, typename UnaryOperation>
auto map(const Image<T>& in, UnaryOperation op)
{
    return mapImage(in, op);
}

template <typename T1, typename T2, typename BinaryOperation>
auto map(const Image<T1>& in1, const Image<T2>& in2, BinaryOperation op)
{
    return mapImage(in1, in2, op);
}

} // namespace komb
