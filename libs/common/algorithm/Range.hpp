#pragma once

#include <iterator>
#include <type_traits>

#include <common/Logging.hpp>

namespace komb {

/// \brief Range represents a half-open range of integers [begin, end).
/// In effect, Range acts like an std::vector<Integer> containing the integers in [begin, end).
/// Note that the range contains the integer begin,
/// but the integer end is just after the last element in the range.
template<typename Integer>
class Range
{
public:
    static_assert(std::is_integral<Integer>::value, "Range should only be used for integers.");

    Range() : begin_(0), end_(0) {}

    Range(Integer begin_arg, Integer end_arg) : begin_(begin_arg), end_(end_arg)
    {
        DCHECK_LE(begin_arg, end_arg);
    }

    using value_type = Integer;

    struct iterator : public std::iterator<std::input_iterator_tag, Integer>
    {
        Integer value;

        explicit iterator(Integer v) : value(v) {}

        Integer operator*() const { return value; }

        iterator& operator++()
        {
            ++value;
            return *this;
        }

        friend bool operator!=(const iterator& a, const iterator& b)
        {
            return a.value != b.value;
        }
    };

    Integer operator[](size_t ix) const { return begin_ + ix; }

    iterator begin() const { return iterator{ begin_ }; }
    iterator end()   const { return iterator{ end_   }; }

    bool empty()   const { return begin_ == end_; }
    size_t size()  const { return static_cast<size_t>(end_ - begin_); }

    Integer front() const { return begin_;   }
    Integer back()  const { return end_ - 1; }

    friend bool operator==(const Range& a, const Range& b)
    {
        return a.begin_ == b.begin_ && a.end_ == b.end_;
    }

    friend bool operator!=(const Range& a, const Range& b)
    {
        return a.begin_ != b.begin_ || a.end_ != b.end_;
    }

    friend bool operator<(const Range& a, const Range& b)
    {
        return (a.begin_ != b.begin_) ? (a.begin_ < b.begin_) : (a.end_ < b.end_);
    }

private:
    Integer begin_, end_;
};

/// for (const auto i : irange(end)) { CHECK(0 <= i && i < end); }
template<typename Integer>
Range<Integer> irange(Integer end)
{
    return {0, end};
}

/// for (const auto i : irange(begin, end)) { CHECK(begin <= i && i < end); }
template<typename Integer>
Range<Integer> irange(Integer begin, Integer end)
{
    return {begin, end};
}

/// for (const auto i : irangeInclusive(first, last)) { CHECK(first <= i && i <= last); }
template<typename Integer>
Range<Integer> irangeInclusive(Integer first, Integer last)
{
    return {first, last + 1};
}

/// for (const auto i : indices(some_vector)) { CHECK(0 <= i && i < some_vector.size(); }
template<typename Container>
auto indices(const Container& container) -> Range<decltype(container.size())>
{
    return {0, container.size()};
}

template<typename Integer, typename Visitor>
void repeat(Integer count, const Visitor& visitor)
{
    CHECK_GE(count, static_cast<Integer>(0));
    while (count != 0)
    {
        visitor();
        count -= 1;
    }
}

} // namespace komb
