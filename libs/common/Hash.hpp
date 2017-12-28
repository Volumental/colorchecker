#pragma once

#include <utility>

#include <boost/filesystem.hpp>
#include <boost/functional/hash.hpp>
#include <Eigen/Core>
#include <Eigen/SparseCore>
#include <opencv2/core.hpp>

#include <common/algorithm/Range.hpp>

namespace komb {

// Based on boost::hash_combine
inline size_t combineHashes(const size_t seed, const size_t new_hash)
{
    return new_hash + 0x9e3779b9 + (seed << 6) + (seed >> 2);
}

template<typename T>
size_t hash(const T& value)
{
    return std::hash<T>()(value);
}

} // namespace komb

// Implementations of hashing functions for some useful types.
namespace std {

template<typename T, size_t N>
struct hash<array<T, N>>
{
    size_t operator()(const array<T, N>& a) const
    {
        hash<T> hasher;
        size_t result = 0;
        for (size_t i = 0; i < N; ++i)
        {
            result = komb::combineHashes(result, hasher(a[i]));
        }
        return result;
    }
};

template<typename T>
struct hash<Eigen::Matrix<T, 2, 1>>
{
    size_t operator()(const Eigen::Matrix<T, 2, 1>& v) const
    {
        hash<T> hasher;
        return komb::combineHashes(hasher(v.x()), hasher(v.y()));
    }
};

template<typename T>
struct hash<Eigen::Matrix<T, 3, 1>>
{
    size_t operator()(const Eigen::Matrix<T, 3, 1>& v) const
    {
        hash<T> hasher;
        size_t result = hasher(v.x());
        result = komb::combineHashes(result, hasher(v.y()));
        result = komb::combineHashes(result, hasher(v.z()));
        return result;
    }
};

template<typename T>
struct hash<Eigen::Matrix<T, Eigen::Dynamic, 1>>
{
    size_t operator()(const Eigen::Matrix<T, Eigen::Dynamic, 1>& vec) const
    {
        size_t result = 0;

        for (const auto col : komb::irange(vec.size()))
        {
            result = komb::combineHashes(result, komb::hash(vec(col)));
        }

        return result;
    }
};

template<typename T>
struct hash<Eigen::SparseMatrix<T>>
{
    size_t operator()(const Eigen::SparseMatrix<T>& mat) const
    {
        size_t result = 0;

        for (const auto k : komb::irange(mat.outerSize()))
        {
            for (typename Eigen::SparseMatrix<T>::InnerIterator it(mat, k); it; ++it)
            {
                result = komb::combineHashes(result, komb::hash(it.value()));
                result = komb::combineHashes(result, komb::hash(it.row()));
                result = komb::combineHashes(result, komb::hash(it.col()));
            }
        }

        return result;
    }
};

template<typename First, typename Second>
struct hash<std::pair<First, Second>>
{
    inline size_t operator()(const std::pair<First, Second>& pair) const
    {
        return komb::combineHashes(hash<First>()(pair.first), hash<Second>()(pair.second));
    }
};

template<> struct hash<boost::filesystem::path> // NOLINT (use namespace fs instead)
{
    size_t operator()(const boost::filesystem::path& p) const // NOLINT (use namespace fs instead)
    {
        return boost::filesystem::hash_value(p); // NOLINT (use namespace fs instead)
    }
};

template<> struct hash<cv::Vec3b>
{
    size_t operator()(const cv::Vec3b& v) const
    {
        return
            (static_cast<size_t>(v[0]) << 16) |
            (static_cast<size_t>(v[1]) <<  8) |
            (static_cast<size_t>(v[2]) <<  0);
    }
};

} // namespace std
