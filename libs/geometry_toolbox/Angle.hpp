#pragma once

#include <cmath>
#include <limits>

#include <Eigen/Geometry>

namespace komb {

const float kPi_f = static_cast<float>(M_PI);
const float kTau_f = 2 * kPi_f;

template<typename T>
constexpr T pi()
{
    return T(M_PI);
}

template<typename T>
constexpr T tau()
{
    return T(2) * T(M_PI);
}

template<typename T>
constexpr T tauInDegrees()
{
    return T(360);
}

template<typename T>
inline T radiansFromDegrees(T degrees)
{
    static_assert(std::numeric_limits<T>::is_integer == false,
        "Use floating point representations instead of integers!");
    return degrees * tau<T>() / tauInDegrees<T>();
}

template<typename T>
inline T degreesFromRadians(T radians)
{
    static_assert(std::numeric_limits<T>::is_integer == false,
        "Use floating point representations instead of integers!");
    return radians * tauInDegrees<T>() / tau<T>();
}

template<typename T>
inline T wrapMinusPiToPi(T radians)
{
    return std::remainder(radians, tau<T>());
}

template<typename T>
inline T wrapZeroToTwoPi(T radians)
{
    return wrapMinusPiToPi(radians - pi<T>()) + pi<T>();
}

// The distance in radians between two rotations in [-pi, +pi]
inline float distance(const Eigen::Rotation2Df& a, const Eigen::Rotation2Df& b)
{
    float angle = (a.inverse() * b).angle();
    angle = wrapMinusPiToPi(angle);
    return std::abs(angle);
}

// The distance in radians between two quaternions in [0, +pi]
inline double angleDifference(Eigen::Quaterniond a, Eigen::Quaterniond b)
{
    a.normalize();
    b.normalize();
    return std::abs(wrapMinusPiToPi(2 * std::acos(a.dot(b))));
}

/// The size of the rotation represented by a quaternion, measured as an angle in [0, +pi].
inline double angleSize(Eigen::Quaterniond q)
{
    q.normalize();
    return std::abs(wrapMinusPiToPi(2 * std::acos(q.w())));
}

/// The size of the rotation matrix, measured as an angle in [0, +pi].
inline double rotationMatrixAngleSize(const Eigen::Matrix3d& m)
{
    return angleSize(Eigen::Quaterniond(m));
}

} // namespace komb
