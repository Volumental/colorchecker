#pragma once

#include <array>

#include "Container.hpp"

namespace komb {

// Apply a unary operation to all elements of an input array
// and output the result as a new array.
template<typename T, size_t N, typename UnaryOperation>
auto map(const std::array<T, N>& in, UnaryOperation op)
    -> std::array<decltype(op(T())), N>
{
    auto out = std::array<decltype(op(T())), N>();
    komb::transform(in, out, op);
    return out;
}

// Apply a binary operation to the corresponding elements of two input arrays
// and output the result as a new array.
template<typename T1, typename T2, size_t N, typename BinaryOperation>
auto map(const std::array<T1, N>& in1, const std::array<T2, N>& in2, BinaryOperation op)
    -> std::array<decltype(op(T1(), T2())), N>
{
    auto out = std::array<decltype(op(T1(), T2())), N>();
    komb::transform(in1, in2, out, op);
    return out;
}

template <typename Container, typename Index, size_t N>
auto slice(const Container& values, const std::array<Index, N>& indices)
    -> std::array<typename Container::value_type, N>
{
    using namespace std;
    static_assert(is_integral<Index>::value, "Index has to be an integral type");
    auto pick_value_with_index = [&values](Index i)
    {
        return values[i];
    };
    return map(indices, pick_value_with_index);
}

} // namespace komb
