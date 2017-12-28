#pragma once

#include <algorithm>
#include <cmath>
#include <initializer_list>
#include <type_traits>
#include <vector>

#include <aaa/aaa.hpp>

#include <common/Logging.hpp>

#include "Container.hpp"
#include "Set.hpp"

namespace komb {

template <typename Container>
using non_const_value_type = typename std::remove_const<typename Container::value_type>::type;

/// Make a std::vector with std::allocator from an arbitrary container.
template <typename Container>
std::vector<non_const_value_type<Container>> makeVector(const Container& in)
{
    using namespace std;
    return vector<non_const_value_type<Container>>(begin(in), end(in));
}

/// Apply a unary operation to the elements and return the result.
template <typename ContainerOut, typename ContainerIn, typename UnaryOperation>
auto mapTo(const ContainerIn& in, UnaryOperation op)
{
    using namespace std;
    ContainerOut out;
    out.reserve(in.size());
    transform(begin(in), end(in), back_inserter(out), op);
    return out;
}

/// Apply a unary operation to the elements and return the result.
/// The input and output containers are of the same kind, including allocators,
/// but the value_type changes based on the unary operation.
template <template<typename> class Allocator, typename T, typename UnaryOperation>
auto map(const std::vector<T, Allocator<T>>& in, UnaryOperation op)
{
    using value_type_out = decltype(op(*begin(in)));
    using allocator_out = Allocator<value_type_out>;
    using container_out = std::vector<value_type_out, allocator_out>;
    return mapTo<container_out>(in, op);
}

/// Apply a binary operation to the elements and return the result.
/// The input and output containers are of the same kind, including allocators,
/// but the value_type changes based on the binary operation.
template <template<typename> class Allocator, typename S, typename T, typename BinaryOperation>
auto map(
    const std::vector<S, Allocator<S>>& in1,
    const std::vector<T, Allocator<T>>& in2,
    BinaryOperation op)
{
    using namespace std;
    using value_type_out = decltype(op(*begin(in1), *begin(in2)));
    using allocator_out = Allocator<value_type_out>;
    CHECK_EQ(in1.size(), in2.size());
    auto out = vector<value_type_out, allocator_out>{};
    out.reserve(in1.size());
    transform(begin(in1), end(in1), begin(in2), back_inserter(out), op);
    return out;
}

/// Apply a unary operation to the elements and return the result.
/// The input container is arbitrary and the output container is std::vector,
/// with std::allocator.
template <typename Container, typename UnaryOperation>
auto mapVector(const Container& in, UnaryOperation op)
    -> std::vector<decltype(op(*std::begin(in)))>
{
    using namespace std;
    using value_type_out = decltype(op(*begin(in)));
    auto out = vector<value_type_out>{};
    out.reserve(in.size());
    transform(begin(in), end(in), back_inserter(out), op);
    return out;
}

/// Apply a binary operation to the elements and return the result.
/// The input containers are arbitrary and the output container is std::vector,
/// with std::allocator.
template <typename ContainerIn1, typename ContainerIn2, typename BinaryOperation>
auto mapVector(const ContainerIn1& in1, const ContainerIn2& in2, BinaryOperation op)
    -> std::vector<decltype(op(*std::begin(in1), *std::begin(in2)))>
{
    using namespace std;
    using value_type_out = decltype(op(*begin(in1), *begin(in2)));
    CHECK_EQ(in1.size(), in2.size());
    auto out = vector<value_type_out>{};
    out.reserve(in1.size());
    transform(begin(in1), end(in1), begin(in2), back_inserter(out), op);
    return out;
}

/// Copy the elements of a container if predicate(element) == true.
/// The output has the same type as the input, including allocator.
template <typename T, typename A, typename Predicate>
std::vector<T, A> filter(const std::vector<T, A>& in, Predicate predicate)
{
    using namespace std;
    auto out = vector<T, A>();
    copy_if(begin(in), end(in), back_inserter(out), predicate);
    return out;
}

/// Copy the elements of a container if predicate(element) == true.
/// The input container is arbitrary and the output container is std::vector,
/// with std::allocator.
template <typename Container, typename Predicate>
auto filterVector(const Container& in, Predicate predicate)
    -> std::vector<typename Container::value_type>
{
    using namespace std;
    using value_type = typename Container::value_type;
    auto out = vector<value_type>();
    copy_if(begin(in), end(in), back_inserter(out), predicate);
    return out;
}

/// Apply a function to each element. If it is boost::none or nullptr, discard, else dereference it.
template <typename ContainerOut, typename ContainerIn, typename UnaryOperation>
ContainerOut filterMapTo(const ContainerIn& inputs, UnaryOperation op)
{
    ContainerOut results;
    for (auto&& input_value : inputs)
    {
        if (auto optional_value = op(input_value))
        {
            results.emplace_back(std::move(*optional_value));
        }
    }
    return results;
}

/// Apply a function to each element. If it is boost::none or nullptr, discard, else dereference it.
template <typename ContainerIn, typename UnaryOperation>
auto filterMap(const ContainerIn& inputs, UnaryOperation op)
{
    using namespace std;
    using result_type = decltype(*op(*begin(inputs)));
    using value_type = typename remove_const<typename remove_reference<result_type>::type>::type;
    using ContainerOut = vector<value_type>;
    return filterMapTo<ContainerOut>(inputs, op);
}

/// Keep *element if (element), so this is  std::vector<boost::optional<X>>  ->  std::vector<X>
template <typename Container>
auto filterOptionals(const Container& inputs)
{
    return filterMap(inputs, [](const auto& value){ return value; });
}

// In matlab, this would be v = v(1:step:end)
template <typename T, typename A>
void subsampleVector(std::vector<T, A>& v, size_t step)
{
    if (step <= 1)
    {
        return;
    }

    size_t new_size(0);
    for (size_t i = 0; i < v.size(); i += step)
    {
        v[new_size] = v[i];
        ++new_size;
    }

    v.resize(new_size);
}

template <typename T, typename A, typename Container>
std::vector<T, A> slice(const std::vector<T, A>& v, const Container& indices)
{
    using Index = typename Container::value_type;
    static_assert(std::is_integral<Index>::value,
        "Index has to be an integral type");
    auto result = std::vector<T, A>(indices.size());
    komb::transform(indices, result, [&v](Index i){return v.at(i);});
    return result;
}

template <typename Container>
auto makeVectorSortedAndUnique(const Container& container)
    -> std::vector<non_const_value_type<Container>>
{
    return makeVector(makeSet(container));
}

/// Flatten an std::vector<Container<T>> to std::vector<T>. All inner elements are kept.
template <typename ContainerOfVectors>
typename ContainerOfVectors::value_type flattenVector(
    const ContainerOfVectors& containers_of_vectors)
{
    typename ContainerOfVectors::value_type flat_vector;
    flat_vector.reserve(aaa::sum(map(containers_of_vectors,
        size<typename ContainerOfVectors::value_type>)));

    for (const auto& container : containers_of_vectors)
    {
        for (const auto& element : container)
        {
            flat_vector.push_back(element);
        }
    }
    return flat_vector;
}

/// Flatten an std::array<Container<T>> to std::vector<T>. All inner elements are kept.
template <typename Container, size_t N>
std::vector<typename Container::value_type>
flattenArray(const std::array<Container, N>& array_of_containers)
{
    using value_type      = typename Container::value_type;
    const auto sizes      = map(array_of_containers, size<Container>);
    const auto total_size = aaa::sum(sizes);
    auto flat_vector      = std::vector<value_type>();
    flat_vector.reserve(total_size);

    for (const auto& container : array_of_containers)
    {
        for (const auto& element : container)
        {
            flat_vector.push_back(element);
        }
    }
    return flat_vector;
}

template <typename T, typename A>
std::vector<T, A> concat(std::initializer_list<std::vector<T, A>> vectors)
{
    std::vector<T, A> result;
    for (const auto& vector : vectors)
    {
        result.insert(end(result), begin(vector), end(vector));
    }
    return result;
}

template<typename T>
T roundToNearest(T value, const std::vector<T>& alternatives)
{
    CHECK(!alternatives.empty());
    T closest = alternatives.front();
    for (const auto& alternative : alternatives)
    {
        if (std::fabs(value - alternative) < std::fabs(value - closest))
        {
            closest = alternative;
        }
    }
    return closest;
}

template <typename T, typename A>
std::vector<T, A> sorted(std::vector<T, A> vector)
{
    std::sort(vector.begin(), vector.end());
    return vector;
}

} // namespace komb
