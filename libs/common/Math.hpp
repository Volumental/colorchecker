#pragma once

#include <algorithm>
#include <cstdint>

#define _USE_MATH_DEFINES
#include <cmath>

#include "FloatingPointConstants.hpp"
#include "Logging.hpp"

namespace komb {

template <typename T>
inline const T& clamp(const T& value, const T& min, const T& max)
{
    CHECK_LE(min, max);
    return value < min ? min :
           value > max ? max :
           value;
}

inline int roundToInt(float x)
{
    return static_cast<int>(std::round(x));
}

inline int floorToInt(float x)
{
    return static_cast<int>(std::floor(x));
}

inline int ceilToInt(float x)
{
    return static_cast<int>(std::ceil(x));
}

inline int roundToInt(double x)
{
    return static_cast<int>(std::round(x));
}

inline int floorToInt(double x)
{
    return static_cast<int>(std::floor(x));
}

inline int ceilToInt(double x)
{
    return static_cast<int>(std::ceil(x));
}

inline size_t roundToSize(float x)
{
    return static_cast<size_t>(std::max(std::round(x), 0.f));
}

inline uint8_t roundToByte(float f)
{
    return static_cast<uint8_t>(clamp(roundToInt(f), 0, 255));
}

inline size_t floorToSize(float x)
{
    return static_cast<size_t>(std::max(std::floor(x), 0.f));
}

inline size_t ceilToSize(float x)
{
    return static_cast<size_t>(std::max(std::ceil(x), 0.f));
}

inline size_t roundToSize(double x)
{
    return static_cast<size_t>(std::max(std::round(x), 0.0));
}

inline size_t floorToSize(double x)
{
    return static_cast<size_t>(std::max(std::floor(x), 0.0));
}

inline size_t ceilToSize(double x)
{
    return static_cast<size_t>(std::max(std::ceil(x), 0.0));
}

inline size_t clampToSize(int x)
{
    return static_cast<size_t>(std::max(x, 0));
}

inline float floatFromDouble(double x)
{
    return static_cast<float>(x);
}

template<typename T>
inline T sqr(const T& x)
{
    return x * x;
}

template<typename T, typename F>
inline T lerp(const T& a, const T& b, const F& t)
{
    return static_cast<T>((F(1) - t) * a + t * b);
}

inline uint8_t lerp(uint8_t a, uint8_t b, float t)
{
    return static_cast<uint8_t>(roundToInt(
        lerp(static_cast<float>(a), static_cast<float>(b), t)));
}

template<typename T>
inline T remap(T x, T in_min, T in_max, T out_min, T out_max)
{
    static_assert(std::is_floating_point<T>::value, "Should be floating point");
    const auto t = (x - in_min) / (in_max - in_min);
    return lerp(out_min, out_max, t);
}

template<typename T>
inline T remapClamp(T x, T in_min, T in_max, T out_min, T out_max)
{
    static_assert(std::is_floating_point<T>::value, "Should be floating point");
    auto t = (x - in_min) / (in_max - in_min);
    t = clamp(t, T(0), T(1));
    return lerp(out_min, out_max, t);
}

template<typename ContainerIn, typename ContainerOut, typename T>
void remap(T in_min, T in_max, T out_min, T out_max,
    const ContainerIn& in_container, ContainerOut& out_container)
{
    const auto f = [=](T x) {return remap(x, in_min, in_max, out_min, out_max);};
    transform(in_container, out_container, f);
}

template<typename ContainerIn, typename ContainerOut, typename T>
void remapClamp(T in_min, T in_max, T out_min, T out_max,
    const ContainerIn& in_container, ContainerOut& out_container)
{
    const auto f = [=](T x) {return remapClamp(x, in_min, in_max, out_min, out_max);};
    transform(in_container, out_container, f);
}

template <typename T>
inline T min3(const T& a, const T& b, const T& c)
{
    return std::min(a, std::min(b, c));
}

template<typename T>
inline T max3(const T& a, const T& b, const T& c)
{
    return std::max(a, std::max(b, c));
}

/// Probability density function for Gaussian normal distribution.
inline float guassianPdf(float x, float mean, float stddev)
{
    float variance = stddev * stddev;
    return std::exp(-sqr(x - mean) / (2.0f * variance)) /
           std::sqrt(2.0f * variance * static_cast<float>(M_PI));
}

} // namespace komb
