#pragma once

#include <algorithm>
#include <array>
#include <cassert>

namespace komb {

// Forward declarations:
template<typename T, size_t D> class ArrayPointer;
template<size_t size> inline size_t product(const size_t* array);
inline size_t product(const size_t* array, size_t size);

// If you want implicit conversions from your specific multidimensional array
// type to ArrayPointer, you can overload this function.
template<typename Array>
size_t multidimensionalSize(const Array& array, size_t dimension)
{
    if (dimension == 0)
    {
        return array.size();
    }
    return 1;
}

template<typename T, size_t D>
size_t multidimensionalSize(const ArrayPointer<T, D>& array, size_t dimension)
{
    return array.size(dimension);
}

/**
* \brief ArrayPointer<T, D> points to an array / random-access-container with
* D dimensions, where each element has type T.
*
* The default value is D=1. For images D=2 and for voxel volumes D=3.
*
* A single-dimensional ArrayPointer<T, 1> can be created automatically from:
* * std::vector
* * std::array
* * std::valarray
* * Eigen::Matrix
*
* Note that ArrayPointer does not own the object it points to. It is a light
* wrapper object that only points to an array owned by someone else.
* It can point to constant data by setting e.g. T = const float.
* It provides an abstract array interface via the member functions:
* * begin()
* * end()
* * data()
* * size()
*
* A user of ArrayPointer can therefore have polymorphic behavior and deal with
* general arrays, without using templates or inheritance.
* ArrayPointer can be used in range based for-loops.
*
* A multi-dimensional ArrayPointer<T, D> can be used like this:
* \code
* size_t width  = 640;
* size_t height = 480;
* size_t depth  = 256;
* ArrayPointer<float, 3> array(data, width, height, depth);
* for (auto& x : array)
* {
*     f(x);
* }
* for (size_t i = 0; i < array.size(); ++i)
* {
*     f(array[i]);
* }
* for (size_t z = 0; z < array.size(2); ++z)
* {
*     for (size_t y = 0; y < array.size(1); ++y)
*     {
*         for (size_t x = 0; x < array.size(0); ++x)
*         {
*             array(x, y, z) = f(x, y, z);
*         }
*     }
* }
* \endcode
*/
template<typename T, size_t D = 1>
class ArrayPointer
{
public:
    using value_type     = T;
    using iterator       = T*;
    using const_iterator = const T*;

    ArrayPointer()
        : data_(nullptr)
        , size_(0)
    {
        std::fill(sizes_.begin(), sizes_.end(), 0);
    }

    template<typename Array>
    ArrayPointer(Array& array) // NOLINT: intentionally implicit
        : data_(array.data())
        , size_(array.size())
    {
        initialize(array);
    }

    template<typename Array>
    ArrayPointer(const Array& array) // NOLINT: intentionally implicit
        : data_(array.data())
        , size_(array.size())
    {
        initialize(array);
    }

    ArrayPointer(T* data_arg, size_t size_0) : data_(data_arg)
    {
        static_assert(D == 1, "Wrong dimensionality of the size");
        sizes_[0] = size_0;
        size_ = product<D>(&sizes_.front());
        assert(data_ != nullptr || size_ == 0);
    }

    ArrayPointer(T* data_arg, size_t width, size_t height) : data_(data_arg)
    {
        static_assert(D == 2, "Wrong dimensionality of the size");
        sizes_[0] = width;
        sizes_[1] = height;
        size_ = product<D>(&sizes_.front());
        assert(data_ != nullptr || size_ == 0);
    }

    ArrayPointer(T* data_arg, size_t width, size_t height, size_t depth) : data_(data_arg)
    {
        static_assert(D == 3, "Wrong dimensionality of the size");
        sizes_[0] = width;
        sizes_[1] = height;
        sizes_[2] = depth;
        size_ = product<D>(&sizes_.front());
        assert(data_ != nullptr || size_ == 0);
    }

    /** \brief The first argument points to the data of the array.
    * After that comes an array with the sizes for each dimension of the array.
    */
    ArrayPointer(T* data_arg, const std::array<size_t, D>& sizes)
        : data_(data_arg), sizes_(sizes)
    {
        size_ = product<D>(&sizes_.front());
        assert(data_ != nullptr || size_ == 0);
    }

    inline T* begin()    const { return data_; }
    inline T* end()      const { return data_ + size_; }
    inline T* data()     const { return data_; }
    inline size_t size() const { return size_; }
    inline size_t size(size_t dimension) const
    {
        if (dimension < D)
        {
            return sizes_[dimension];
        }
        return 1;
    }
    const std::array<size_t, D>& sizes() const { return sizes_; }

    inline bool empty() const {return size_ == 0;}

    /// Linear access.
    inline T& operator[](size_t i) const
    {
        assert(i < size_);
        return data_[i];
    }

    inline T& operator()(size_t index_0) const
    {
        static_assert(D == 1, "Wrong dimensionality of the index");
        assert(index_0 < sizes_[0]);
        return data_[index_0];
    }

    inline T& operator()(size_t x, size_t y) const
    {
        static_assert(D == 2, "Wrong dimensionality of the index");
        assert(x < sizes_[0]);
        assert(y < sizes_[1]);
        return data_[x + sizes_[0] * y];
    }

    inline T& operator()(size_t x, size_t y, size_t z) const
    {
        static_assert(D == 3, "Wrong dimensionality of the index");
        assert(x < sizes_[0]);
        assert(y < sizes_[1]);
        assert(z < sizes_[2]);
        return data_[x + sizes_[0] * (y + sizes_[1] * z)];
    }

    /// Returns the linear index of the given coordinate
    inline size_t index(size_t x, size_t y, size_t z) const
    {
        static_assert(D == 3, "Wrong dimensionality of the index");
        assert(x < sizes_[0]);
        assert(y < sizes_[1]);
        assert(z < sizes_[2]);
        return x + sizes_[0] * (y + sizes_[1] * z);
    }

private:
    ArrayPointer(T* data_arg, const size_t* sizes, size_t total_size)
        : data_(data_arg)
        , size_(total_size)
    {
        assert(data_ != nullptr || size_ == 0);
        std::copy(sizes, sizes + D, sizes_.begin());
    }

    template<typename Array>
    void initialize(const Array& array)
    {
        assert(data_ != nullptr || size_ == 0);
        for (size_t i = 0; i < D; ++i)
        {
            sizes_[i] = multidimensionalSize(array, i);
        }
    }

    T* data_;
    size_t size_;
    std::array<size_t, D> sizes_;
};

template<> inline
size_t product<0>(const size_t*)
{
    return 1;
}

template<size_t size> inline
size_t product(const size_t* array)
{
    return array[0] * product<size - 1>(array + 1);
}

inline
size_t product(const size_t* array, size_t size)
{
    size_t x = 1;
    for (size_t i = 0; i < size; ++i)
    {
        x *= array[i];
    }
    return x;
}

template <typename T> inline
size_t index(ArrayPointer<T, 3> array, size_t x, size_t y, size_t z)
{
    return z * array.size(0) * array.size(1) + y * array.size(0) + x;
}

//// Takes an ArrayPointer<T, 2> and returns an ArrayPointer<T, 1> to one of the rows.
template<typename T>
ArrayPointer<T> slice(ArrayPointer<T, 2> array, size_t y)
{
    return ArrayPointer<T>(&array(0, y), array.size(0));
}

namespace image
{

template<typename T> size_t  width(const ArrayPointer<T, 2>& array) { return array.size(0); }
template<typename T> size_t height(const ArrayPointer<T, 2>& array) { return array.size(1); }
template<typename T> size_t   size(const ArrayPointer<T, 2>& array) { return array.size();  }
template<typename T> T*       data(const ArrayPointer<T, 2>& array) { return array.data();  }

template<typename T>
std::array<size_t, 2> dimensions(const ArrayPointer<T, 2>& array)
{
    return {array.size(0), array.size(1)};
}

} // namespace image

} // namespace komb
