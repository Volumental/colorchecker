#pragma once
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wfloat-equal"

#include <limits>

namespace komb {

const auto kInfinityFloat  = std::numeric_limits<float>::infinity();
const auto kInfinityDouble = std::numeric_limits<double>::infinity();
const auto kNaNFloat       = std::numeric_limits<float>::quiet_NaN();
const auto kNaNDouble      = std::numeric_limits<double>::quiet_NaN();

template<typename T>
constexpr bool isPositiveInfinity(T value)
{
    using namespace std;
    static_assert(numeric_limits<T>::is_iec559, "Non-standard floating point number.");
    static_assert(numeric_limits<T>::has_infinity, "Missing infinity.");
    return value == numeric_limits<T>::infinity();
}

template<typename T>
constexpr bool isNegativeInfinity(T value)
{
    using namespace std;
    static_assert(numeric_limits<T>::is_iec559, "Non-standard floating point number.");
    static_assert(numeric_limits<T>::has_infinity, "Missing infinity.");
    return value == -numeric_limits<T>::infinity();
}

template<typename T>
constexpr bool isZero(T value)
{
    return value == 0;
}

// Test isZero<float>:
static_assert(isZero(+0.f),                                        "Bad zero 0f");
static_assert(isZero(-0.f),                                        "Bad zero 1f");
static_assert(!isZero(+std::numeric_limits<float>::min()),         "Bad zero 2f");
static_assert(!isZero(-std::numeric_limits<float>::min()),         "Bad zero 3f");
static_assert(!isZero(+std::numeric_limits<float>::denorm_min()),  "Bad zero 4f");
static_assert(!isZero(-std::numeric_limits<float>::denorm_min()),  "Bad zero 5f");
static_assert(!isZero(+std::numeric_limits<float>::quiet_NaN()),   "Bad zero 6f");
static_assert(!isZero(-std::numeric_limits<float>::quiet_NaN()),   "Bad zero 7f");

// Test isZero<double>:
static_assert(isZero(+0.0),                                        "Bad zero 0d");
static_assert(isZero(-0.0),                                        "Bad zero 1d");
static_assert(!isZero(+std::numeric_limits<double>::min()),        "Bad zero 2d");
static_assert(!isZero(-std::numeric_limits<double>::min()),        "Bad zero 3d");
static_assert(!isZero(+std::numeric_limits<double>::denorm_min()), "Bad zero 4d");
static_assert(!isZero(-std::numeric_limits<double>::denorm_min()), "Bad zero 5d");
static_assert(!isZero(+std::numeric_limits<double>::quiet_NaN()),  "Bad zero 6d");
static_assert(!isZero(-std::numeric_limits<double>::quiet_NaN()),  "Bad zero 7d");

// Test isPositiveInfinity<float>:
static_assert(isPositiveInfinity(+std::numeric_limits<float>::infinity()),    "Bad +inf 0f");
static_assert(!isPositiveInfinity(-std::numeric_limits<float>::infinity()),   "Bad +inf 1f");
static_assert(!isPositiveInfinity(+std::numeric_limits<float>::max()),        "Bad +inf 2f");
static_assert(!isPositiveInfinity(-std::numeric_limits<float>::max()),        "Bad +inf 3f");
static_assert(!isPositiveInfinity(+std::numeric_limits<float>::lowest()),     "Bad +inf 4f");
static_assert(!isPositiveInfinity(-std::numeric_limits<float>::lowest()),     "Bad +inf 5f");
static_assert(!isPositiveInfinity(+std::numeric_limits<float>::quiet_NaN()),  "Bad +inf 6f");
static_assert(!isPositiveInfinity(-std::numeric_limits<float>::quiet_NaN()),  "Bad +inf 7f");

// Test isPositiveInfinity<double>:
static_assert(isPositiveInfinity(+std::numeric_limits<double>::infinity()),   "Bad +inf 0d");
static_assert(!isPositiveInfinity(-std::numeric_limits<double>::infinity()),  "Bad +inf 1d");
static_assert(!isPositiveInfinity(+std::numeric_limits<double>::max()),       "Bad +inf 2d");
static_assert(!isPositiveInfinity(-std::numeric_limits<double>::max()),       "Bad +inf 3d");
static_assert(!isPositiveInfinity(+std::numeric_limits<double>::lowest()),    "Bad +inf 4d");
static_assert(!isPositiveInfinity(-std::numeric_limits<double>::lowest()),    "Bad +inf 5d");
static_assert(!isPositiveInfinity(+std::numeric_limits<double>::quiet_NaN()), "Bad +inf 6d");
static_assert(!isPositiveInfinity(-std::numeric_limits<double>::quiet_NaN()), "Bad +inf 7d");

// Test isNegativeInfinity<float>:
static_assert(!isNegativeInfinity(+std::numeric_limits<float>::infinity()),   "Bad -inf 0f");
static_assert(isNegativeInfinity(-std::numeric_limits<float>::infinity()),    "Bad -inf 1f");
static_assert(!isNegativeInfinity(+std::numeric_limits<float>::max()),        "Bad -inf 2f");
static_assert(!isNegativeInfinity(-std::numeric_limits<float>::max()),        "Bad -inf 3f");
static_assert(!isNegativeInfinity(+std::numeric_limits<float>::lowest()),     "Bad -inf 4f");
static_assert(!isNegativeInfinity(-std::numeric_limits<float>::lowest()),     "Bad -inf 5f");
static_assert(!isNegativeInfinity(+std::numeric_limits<float>::quiet_NaN()),  "Bad -inf 6f");
static_assert(!isNegativeInfinity(-std::numeric_limits<float>::quiet_NaN()),  "Bad -inf 7f");

// Test isNegativeInfinity<double>:
static_assert(!isNegativeInfinity(+std::numeric_limits<double>::infinity()),  "Bad -inf 0d");
static_assert(isNegativeInfinity(-std::numeric_limits<double>::infinity()),   "Bad -inf 1d");
static_assert(!isNegativeInfinity(+std::numeric_limits<double>::max()),       "Bad -inf 2d");
static_assert(!isNegativeInfinity(-std::numeric_limits<double>::max()),       "Bad -inf 3d");
static_assert(!isNegativeInfinity(+std::numeric_limits<double>::lowest()),    "Bad -inf 4d");
static_assert(!isNegativeInfinity(-std::numeric_limits<double>::lowest()),    "Bad -inf 5d");
static_assert(!isNegativeInfinity(+std::numeric_limits<double>::quiet_NaN()), "Bad -inf 6d");
static_assert(!isNegativeInfinity(-std::numeric_limits<double>::quiet_NaN()), "Bad -inf 7d");

} // namespace komb

#pragma GCC diagnostic pop
