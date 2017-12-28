#pragma once

#include <cmath>

#include <common/Logging.hpp>

#include "Types.hpp"

namespace komb {
namespace vector_tools {

/**
 * @brief Test if two vectors are parallel.
 */
template <typename T>
bool parallel(const Vector3<T>& first, const Vector3<T>& second, const T epsillon)
{
    CHECK_GT(epsillon, T(0));
    const T dot = first.normalized().dot(second.normalized());
    return T(1) - std::abs(dot) < epsillon;
}

/**
 * @brief Compute orthogonal direction component of vec.
 *       ^     ^
 *  vec /      |
 *     /       | orthogonal direction
 *    /        |
 *   ----------->
 *    direction
 */
template <typename T>
Vector3<T> orthogonalDirection(const Vector3<T>& vec, const Vector3<T>& direction)
{
    const Vector3<T> unit_direction = direction.normalized();
    const Vector3<T> projection = unit_direction * (unit_direction.dot(vec));
    const Vector3<T> orhtogonal_direction = (vec - projection).normalized();
    return orhtogonal_direction;
}

} // namespace vector_tools
} // namespace komb
