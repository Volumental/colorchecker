#pragma once

#include <algorithm>

#include "traits.hpp"

namespace aaa {

/**
@addtogroup negate

Examples:
```
std::vector<double>   in1 = { 1.1, 2.2, 3.3, 4.4, 5.5 };
std::vector<double>   in2 = { 1.1, 2.2, 3.3, 4.4, 5.5 };
std::valarray<double> in3 = { 1.1, 2.2, 3.3, 4.4, 5.5 };
std::array<double, 5> in4 = { 1.1, 2.2, 3.3, 4.4, 5.5 };
std::array<double, 9> in5 = { 1.1, 2.2, 3.3, 4.4, 5.5, 6.6, 7.7, 8.8, 9.9 };
std::vector<double>   out = { 1.1, 2.2, 3.3, 4.4, 5.5 };

using namespace aaa;

// In this example we want the same type of container
// for both the input and output. We can then return the output by value.
out = negate(in1);

// In this example we mix arbitrary type of containers
// for the input and output. We then use a reference for output.
negate(in3, out);

// In this example we use iterators for the input and output.
negate(begin(in5) + 1, begin(in5) + 6, begin(out));
```

@{
*/

template<typename InputIterator, typename OutputIterator>
void negate(InputIterator first_in, InputIterator last_in, OutputIterator first_out)
{
    auto f = [](const value_type_i<InputIterator>& in) { return -in; };
    std::transform(first_in, last_in, first_out, f);
}

template<typename Container1, typename Container2>
void negate(const Container1& in, Container2& out)
{
    assert(in.size() == out.size());
    using std::begin;
    using std::end;
    negate(begin(in), end(in), begin(out));
}

template<typename Container>
Container negate(const Container& in)
{
    auto out = in;
    negate(in, out);
    return out;
}

/** @} */

} // namespace aaa
