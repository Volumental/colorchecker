#pragma once

#include <random>

namespace komb {

using RandomEngine = std::mt19937_64;

inline RandomEngine getRandomEngine(std::uint_fast64_t seed = 0)
{
    return RandomEngine{seed};
}

} // namespace komb
