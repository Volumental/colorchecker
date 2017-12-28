#pragma once

#include <algorithm>
#include <cassert>

#include "traits.hpp"

namespace aaa {

/**
@addtogroup std_algorithms_container

This module defines container versions of some range
algorithms from the standard library header
[algorithm](http://www.cplusplus.com/reference/algorithm/).
It only does it for the algorithms that are used a lot for arithmetic types.

@{
*/

// TODO: add all_of, any_of, none_of.

template<typename Container1, typename Container2>
void copy(const Container1& in, Container2& out)
{
    assert(in.size() == out.size());
    using std::begin;
    using std::end;
    std::copy(begin(in), end(in), begin(out));
}

template<typename Container, typename Value>
void fill(Container& container, const Value& value)
{
    using std::begin;
    using std::end;
    std::fill(begin(container), end(container), value);
}

template<typename Container>
typename Container::iterator min_element(Container& container)
{
    using std::begin;
    using std::end;
    return std::min_element(begin(container), end(container));
}

template<typename Container>
typename Container::const_iterator min_element(const Container& container)
{
    using std::begin;
    using std::end;
    return std::min_element(begin(container), end(container));
}

template<typename Container, typename Compare>
typename Container::iterator min_element(Container& container, Compare comp)
{
    using std::begin;
    using std::end;
    return std::min_element(begin(container), end(container), comp);
}

template<typename Container, typename Compare>
typename Container::const_iterator min_element(const Container& container, Compare comp)
{
    using std::begin;
    using std::end;
    return std::min_element(begin(container), end(container), comp);
}

template<typename Container>
typename Container::iterator max_element(Container& container)
{
    using std::begin;
    using std::end;
    return std::max_element(begin(container), end(container));
}

template<typename Container>
typename Container::const_iterator max_element(const Container& container)
{
    using std::begin;
    using std::end;
    return std::max_element(begin(container), end(container));
}


template<typename Container, typename Compare>
typename Container::iterator max_element(Container& container, Compare comp)
{
    using std::begin;
    using std::end;
    return std::max_element(begin(container), end(container), comp);
}

template<typename Container, typename Compare>
typename Container::const_iterator max_element(const Container& container, Compare comp)
{
    using std::begin;
    using std::end;
    return std::max_element(begin(container), end(container), comp);
}

template<typename Container>
std::pair<const_iterator<Container>, const_iterator<Container>>
minmax_element(const Container& container)
{
    using std::begin;
    using std::end;
    return std::minmax_element(begin(container), end(container));
}

template<typename Container>
std::pair<iterator<Container>, iterator<Container>>
minmax_element(Container& container)
{
    using std::begin;
    using std::end;
    return std::minmax_element(begin(container), end(container));
}

template<typename Container, typename Compare>
std::pair<const_iterator<Container>, const_iterator<Container>>
minmax_element(const Container& container, Compare comp)
{
    using std::begin;
    using std::end;
    return std::minmax_element(begin(container), end(container), comp);
}

template<typename Container, typename Compare>
std::pair<iterator<Container>, iterator<Container>>
minmax_element(Container& container, Compare comp)
{
    using std::begin;
    using std::end;
    return std::minmax_element(begin(container), end(container), comp);
}

/** @} */

} // namespace aaa
