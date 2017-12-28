#pragma once

#include <algorithm>
#include <functional>

namespace aaa {

/**
@addtogroup logical_and

Example:
```
std::vector<bool>   in1 = { true, false, true, false };
std::vector<bool>   in2 = { true, false, true, false };
std::valarray<bool> in3 = { true, false, true, false };
std::array<bool, 4> in4 = { true, false, true, false };
std::array<bool, 6> in5 = { true, false, true, false, true, false };
std::vector<bool>   out = { true, false, true, false };

using namespace aaa;

out = logical_and(in1, in2); // Same type of containers.
logical_and(in3, in4, out); // Mixed type of containers.
logical_and(begin(in5) + 1, begin(in5) + 5, begin(in4), begin(out)); // Ranges of iterators.
```

@{
*/

template<typename InputIterator1, typename InputIterator2, typename OutputIterator>
void logical_and(InputIterator1 first_left, InputIterator1 last_left, InputIterator2 first_right, OutputIterator first_out)
{
    std::transform(first_left, last_left, first_right, first_out, std::logical_and<bool>());
}

template<typename Container1, typename Container2, typename Container3>
void logical_and(const Container1& left, const Container2& right, Container3& out)
{
    using namespace std;
    aaa::logical_and(begin(left), end(left), begin(right), begin(out));
}

template<typename Container>
Container logical_and(const Container& left, const Container& right)
{
    auto out = left;
    aaa::logical_and(left, right, out);
    return out;
}

/** @} */

} // namespace aaa
