#pragma once

#include <algorithm>
#include <array>
#include <iterator>
#include <numeric>
#include <utility>

#include <common/Logging.hpp>

#include "Iterator.hpp"

namespace komb {

template <typename Container>
size_t size(const Container& container)
{
    using namespace std;
    return static_cast<size_t>(std::distance(begin(container), end(container)));
}

template <typename Container>
bool isEmpty(const Container& container)
{
    return container.empty();
}

template<typename Container, typename UnaryFunction>
void for_each(Container& io_container, UnaryFunction f)
{
    using namespace std;
    std::for_each(begin(io_container), end(io_container), f);
}

template<typename Container, typename UnaryFunction>
void for_each(const Container& container, UnaryFunction f)
{
    using namespace std;
    std::for_each(begin(container), end(container), f);
}

template<typename ContainerIn, typename ContainerOut, typename UnaryOperation>
void transform(const ContainerIn& in, ContainerOut& out, UnaryOperation op)
{
    using namespace std;
    DCHECK_EQ(size(in), size(out));
    std::transform(begin(in), end(in), begin(out), op);
}

template<typename ContainerIn1, typename ContainerIn2, typename ContainerOut,
    typename BinaryOperation>
void transform(
    const ContainerIn1& in1, const ContainerIn2& in2, ContainerOut& out, BinaryOperation op)
{
    using namespace std;
    DCHECK_EQ(size(in1), size(in2));
    DCHECK_EQ(size(in2), size(out));
    std::transform(begin(in1), end(in1), begin(in2), begin(out), op);
}

template<typename ContainerIn, typename ContainerOut>
void copy(const ContainerIn& in, ContainerOut& out)
{
    using namespace std;
    DCHECK_EQ(size(in), size(out));
    std::copy(begin(in), end(in), begin(out));
}

template<typename ContainerIn, typename ContainerOut, typename Predicate>
void copy_if(const ContainerIn& in, ContainerOut& out, Predicate predicate)
{
    using namespace std;
    DCHECK_EQ(size(in), size(out));
    std::copy_if(begin(in), end(in), begin(out), predicate);
}

template<typename Container, typename Predicate>
Container copy_if(const Container& in, Predicate predicate)
{
    using namespace std;
    Container out = in;
    out.erase(std::copy_if(begin(in), end(in), begin(out), predicate), end(out));
    return out;
}

template<typename ContainerOut, typename Value>
void fill(ContainerOut& out, const Value& value)
{
    using namespace std;
    std::fill(begin(out), end(out), value);
}

template<typename Container, typename Value>
auto find(const Container& in, const Value& value) -> decltype(std::begin(in))
{
    using namespace std;
    return find(begin(in), end(in), value);
}

template<typename Container, typename Predicate>
auto find_if(const Container& in, Predicate predicate) -> decltype(std::begin(in))
{
    using namespace std;
    return find_if(begin(in), end(in), predicate);
}

template<typename Container, typename Value>
size_t count(const Container& in, const Value& value)
{
    using namespace std;
    return static_cast<size_t>(std::count(begin(in), end(in), value));
}

template<typename ContainerIn, typename Predicate>
size_t count_if(const ContainerIn& in, Predicate predicate)
{
    using namespace std;
    return static_cast<size_t>(std::count_if(begin(in), end(in), predicate));
}

template<typename Container>
typename Container::iterator min_element(Container& io_container)
{
    using namespace std;
    return std::min_element(begin(io_container), end(io_container));
}

template<typename Container>
typename Container::const_iterator min_element(const Container& container)
{
    using namespace std;
    return std::min_element(begin(container), end(container));
}

template<typename Container, typename Compare>
typename Container::iterator min_element(Container& io_container, Compare comp)
{
    using namespace std;
    return std::min_element(begin(io_container), end(io_container), comp);
}

template<typename Container, typename Compare>
typename Container::const_iterator min_element(const Container& container, Compare comp)
{
    using namespace std;
    return std::min_element(begin(container), end(container), comp);
}

template<typename Container>
typename Container::iterator max_element(Container& io_container)
{
    using namespace std;
    return std::max_element(begin(io_container), end(io_container));
}

template<typename Container>
typename Container::const_iterator max_element(const Container& container)
{
    using namespace std;
    return std::max_element(begin(container), end(container));
}

template<typename Container, typename Compare>
typename Container::iterator max_element(Container& io_container, Compare comp)
{
    using namespace std;
    return std::max_element(begin(io_container), end(io_container), comp);
}

template<typename Container, typename Compare>
typename Container::const_iterator max_element(const Container& container, Compare comp)
{
    using namespace std;
    return std::max_element(begin(container), end(container), comp);
}

template<typename Container, typename Value>
typename Container::iterator remove(Container& io_container, const Value& value)
    __attribute__((warn_unused_result));

template<typename Container, typename Predicate>
typename Container::iterator remove_if(Container& io_container, Predicate predicate)
    __attribute__((warn_unused_result));

template<typename Container, typename Value>
typename Container::iterator remove(Container& io_container, const Value& value)
{
    using namespace std;
    return std::remove(begin(io_container), end(io_container), value);
}

template<typename Container, typename Predicate>
typename Container::iterator remove_if(Container& io_container, Predicate predicate)
{
    using namespace std;
    return std::remove_if(begin(io_container), end(io_container), predicate);
}

template<typename Container, typename Value>
void replace(Container& io_container, const Value& replace_this, const Value& with_this)
{
    using namespace std;
    std::replace(begin(io_container), end(io_container), replace_this, with_this);
}

template<typename Container, typename Predicate, typename Value>
void replace_if(Container& io_container, Predicate predicate, const Value& value)
{
    using namespace std;
    std::replace_if(begin(io_container), end(io_container), predicate, value);
}

template<typename Container, typename Predicate>
bool any_of(const Container& io_container, Predicate predicate)
{
    using namespace std;
    return std::any_of(begin(io_container), end(io_container), predicate);
}

template<typename Container, typename Predicate>
bool all_of(const Container& io_container, Predicate predicate)
{
    using namespace std;
    return std::all_of(begin(io_container), end(io_container), predicate);
}

template<typename Container, typename Predicate>
bool none_of(const Container& io_container, Predicate predicate)
{
    using namespace std;
    return std::none_of(begin(io_container), end(io_container), predicate);
}

template<typename Container>
void sort(Container& io_container)
{
    using namespace std;
    std::sort(begin(io_container), end(io_container));
}

template<typename Container, typename Predicate>
void sort(Container& io_container, Predicate predicate)
{
    using namespace std;
    std::sort(begin(io_container), end(io_container), predicate);
}

/// Sort the given range so elements with lower key(element) comes first.
template<typename Container, typename Key>
void stable_sort_by_key(Container& io_container, const Key& key)
{
    using namespace std;
    std::stable_sort(begin(io_container), end(io_container), [&key](const auto& a, const auto& b)
    {
        return key(a) < key(b);
    });
}

template<typename Container, typename Predicate>
void stable_sort(Container& io_container, Predicate predicate)
{
    using namespace std;
    std::stable_sort(begin(io_container), end(io_container), predicate);
}

template<typename Container>
void reverse(Container& io_container)
{
    using namespace std;
    std::reverse(begin(io_container), end(io_container));
}

template<typename Container>
void nth_element(Container& io_container, size_t nth)
{
    CHECK_LE(nth, io_container.size());
    using namespace std;
    const auto nth_iterator = begin(io_container) + static_cast<int>(nth);
    std::nth_element(begin(io_container), nth_iterator, end(io_container));
}

template<typename Container, typename Predicate>
void nth_element(Container& io_container, size_t nth, Predicate predicate)
{
    CHECK_LE(nth, io_container.size());
    using namespace std;
    const auto nth_iterator = begin(io_container) + static_cast<int>(nth);
    std::nth_element(begin(io_container), nth_iterator, end(io_container), predicate);
}

template<typename Container>
void partial_sort(Container& io_container, size_t nth)
{
    CHECK_LE(nth, io_container.size());
    using namespace std;
    const auto nth_iterator = begin(io_container) + static_cast<int>(nth);
    std::partial_sort(begin(io_container), nth_iterator, end(io_container));
}

template<typename Container, typename Predicate>
void partial_sort(Container& io_container, size_t nth, Predicate predicate)
{
    CHECK_LE(nth, io_container.size());
    using namespace std;
    const auto nth_iterator = begin(io_container) + static_cast<int>(nth);
    std::partial_sort(begin(io_container), nth_iterator, end(io_container), predicate);
}

template<typename Container, typename T>
T accumulate(const Container& container, T init)
{
    using namespace std;
    return std::accumulate(begin(container), end(container), init);
}

template<typename Container, typename T, typename BinaryOperator>
T accumulate(const Container& container, T init, BinaryOperator op)
{
    using namespace std;
    return std::accumulate(begin(container), end(container), init, op);
}

template<typename Container, typename Generator>
void generate(Container& io_container, Generator generator)
{
    using namespace std;
    std::generate(begin(io_container), end(io_container), generator);
}

// ----------------------------------------------------------------------------
// Not part of <algorithm>, but nice to haves:

template<typename ContainerIn, typename ContainerOut>
void cast(const ContainerIn& in, ContainerOut& out)
{
    using namespace std;
    DCHECK_EQ(size(in), size(out));
    cast(begin(in), end(in), begin(out));
}

template<typename Container, typename Value>
bool contains(const Container& container, const Value& value)
{
    using namespace std;
    return contains(begin(container), end(container), value);
}

template<typename Container>
size_t countUnique(const Container& container)
{
    return komb::countUnique(begin(container), end(container));
}

template<typename Container, typename Value>
size_t erase(Container& io_container, const Value& value)
{
    using namespace std;
    auto it = remove(io_container, value);
    const auto num_erased = distance(it, end(io_container));
    io_container.erase(it, end(io_container));
    return static_cast<size_t>(num_erased);
}

template<typename Container, typename Predicate>
size_t erase_if(Container& io_container, Predicate predicate)
{
    using namespace std;
    auto it = remove_if(io_container, predicate);
    const auto num_erased = distance(it, end(io_container));
    io_container.erase(it, end(io_container));
    return static_cast<size_t>(num_erased);
}

template<typename Container>
void eraseIndex(Container& io_container, size_t index)
{
    auto it = io_container.begin();
    std::advance(it, index);
    io_container.erase(it);
}

template<typename Container1, typename Container2>
void append(Container2& io_destination, const Container1& source)
{
    using namespace std;
    io_destination.insert(end(io_destination), begin(source), end(source));
}

template <typename Container, typename Evaluator>
auto pickLargestIt(Container&& alternatives, const Evaluator& eval)
    -> decltype(std::begin(alternatives))
{
    using namespace std;
    CHECK(!alternatives.empty());
    return pickLargestIt(begin(alternatives), end(alternatives), eval);
}

template <typename Container, typename Evaluator>
auto pickLargest(Container&& alternatives, const Evaluator& eval)
    -> decltype(*std::begin(alternatives))
{
    CHECK(!alternatives.empty());
    return *pickLargestIt(alternatives, eval);
}

template <typename Container>
auto pickLargest(Container&& alternatives)
    -> decltype(*std::begin(alternatives))
{
    CHECK(!alternatives.empty());
    return *max_element(alternatives);
}

template <typename Container, typename Evaluator>
auto pickSmallestIt(Container&& alternatives, const Evaluator& eval)
    -> decltype(std::begin(alternatives))
{
    using namespace std;
    CHECK(!alternatives.empty());
    return pickSmallestIt(begin(alternatives), end(alternatives), eval);
}

template <typename Container, typename Evaluator>
size_t pickSmallestIndex(Container&& alternatives, const Evaluator& eval)
{
    using namespace std;
    return distance(begin(alternatives), pickSmallestIt(alternatives, eval));
}

template <typename Container, typename Evaluator>
auto pickSmallest(Container&& alternatives, const Evaluator& eval)
    -> decltype(*std::begin(alternatives))
{
    CHECK(!alternatives.empty());
    return *pickSmallestIt(alternatives, eval);
}

template <typename Container>
auto pickSmallest(Container&& alternatives)
    -> decltype(*std::begin(alternatives))
{
    CHECK(!alternatives.empty());
    return *min_element(alternatives);
}

/// Return the smallest and second smallest elements.
template <typename Container, typename Evaluator>
auto pickSmallestTwo(Container&& alternatives, const Evaluator& eval)
    -> std::array<std::pair<decltype(std::begin(alternatives)),
    decltype(eval(*std::begin(alternatives)))>, 2>
{
    using namespace std;
    CHECK_GE(size(alternatives), 2u);
    return pickSmallestTwo(begin(alternatives), end(alternatives), eval);
}

template <typename Container>
void sortUniqInplace(Container& io_container)
{
    sort(io_container);
    using namespace std;
    io_container.erase(std::unique(begin(io_container), end(io_container)), end(io_container));
}

template<typename Container, typename Compare>
void sortUniqInplace(Container& io_container, Compare compare)
{
    sort(io_container, compare);
    using namespace std;
    io_container.erase(std::unique(begin(io_container), end(io_container)), end(io_container));
}

template <typename Container>
bool equalAfterSort(Container a, Container b)
{
    if (a.size() != b.size())
    {
        return false;
    }
    sort(a);
    sort(b);
    return a == b;
}

} // namespace komb
