#pragma once

#include <algorithm>
#include <array>
#include <iterator>
#include <numeric>
#include <utility>
#include <vector>

#include <common/Logging.hpp>

namespace komb {

template<typename InputIterator, typename OutputIterator>
void cast(InputIterator in_begin, InputIterator in_end, OutputIterator out_begin)
{
    using InputElement = typename std::iterator_traits<InputIterator>::value_type;
    using OutputElement = typename std::iterator_traits<OutputIterator>::value_type;
    auto castFunction = [](const InputElement& element) -> OutputElement
    {
        return static_cast<OutputElement>(element);
    };
    std::transform(in_begin, in_end, out_begin, castFunction);
}

template<typename InputIterator, typename Value>
bool contains(InputIterator in_begin, InputIterator in_end, const Value& value)
{
    return std::find(in_begin, in_end, value) != in_end;
}

template <typename InputIterator, typename Evaluator>
InputIterator pickLargestIt(
    InputIterator in_begin, InputIterator in_end, const Evaluator& eval)
{
    CHECK(in_begin != in_end);

    auto it = in_begin;
    auto best_element = it;
    auto best_value = eval(*it);

    ++it;

    for (; it != in_end; ++it)
    {
        float value = eval(*it);
        if (value > best_value)
        {
            best_value = value;
            best_element = it;
        }
    }

    return best_element;
}

template <typename InputIterator, typename Evaluator>
InputIterator pickSmallestIt(
    InputIterator in_begin, InputIterator in_end, const Evaluator& eval)
{
    CHECK(in_begin != in_end);

    auto it = in_begin;
    auto best_element = it;
    auto best_value = eval(*it);

    ++it;

    for (; it != in_end; ++it)
    {
        float value = eval(*it);
        if (value < best_value)
        {
            best_value = value;
            best_element = it;
        }
    }

    return best_element;
}

/// Return the smallest and second smallest elements.
template <typename InputIterator, typename Evaluator>
auto pickSmallestTwo(InputIterator in_begin, InputIterator in_end, const Evaluator& eval)
    -> std::array<std::pair<InputIterator, decltype(eval(*in_begin))>, 2>
{
    CHECK_GE(std::distance(in_begin, in_end), 2u);

    auto it = in_begin;

    auto best_it    = it;
    auto best_value = eval(*it);
    ++it;

    auto second_it    = it;
    auto second_value = eval(*it);
    ++it;

    if (second_value < best_value)
    {
        std::swap(best_it,    second_it);
        std::swap(best_value, second_value);
    }

    for (; it != in_end; ++it)
    {
        float value = eval(*it);

        if (value < best_value)
        {
            second_it    = best_it;
            second_value = best_value;

            best_it    = it;
            best_value = value;
        }
        else if (value < second_value)
        {
            second_it    = it;
            second_value = value;
        }
    }

    return {
        std::make_pair(best_it,   best_value),
        std::make_pair(second_it, second_value),
    };
}

template<typename ConstInputIterator>
size_t countUnique(ConstInputIterator in_begin, ConstInputIterator in_end)
{
    using InputElement = typename std::iterator_traits<ConstInputIterator>::value_type;
    std::vector<InputElement> sorted(in_begin, in_end);
    std::sort(begin(sorted), end(sorted));
    auto unique_end = std::unique(begin(sorted), end(sorted));
    return std::distance(begin(sorted), unique_end);
}

} // namespace komb
