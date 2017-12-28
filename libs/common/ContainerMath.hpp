#pragma once

#include <memory>
#include <vector>

#include "algorithm/Range.hpp"
#include "Math.hpp"

namespace komb {

template <typename T, typename A = std::allocator<T>>
std::vector<T, A> linspace(T from_inclusive, T to, size_t num_points,
    bool to_inclusive = true)
{
    double interpolation_interval = static_cast<double>(num_points);
    if (to_inclusive)
    {
        CHECK_GE_F(num_points, 2u, "Need at least two points in [closed, closed] linspace");
        interpolation_interval -= 1.0;
    }
    else
    {
        CHECK_GE_F(num_points, 1u, "Need at least one point in [closed, open) linspace");
    }

    std::vector<T, A> result(num_points);
    for (auto i : irange(num_points))
    {
        double t = static_cast<double>(i) / interpolation_interval;
        result[i] = lerp(from_inclusive, to, t);
    }

    return result;
}

} // namespace komb
