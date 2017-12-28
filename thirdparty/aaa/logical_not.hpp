#pragma once

#include <algorithm>
#include <functional>

namespace aaa {

/**
@addtogroup logical_not

Example:
```
std::vector<bool>   in1 = { true, false, true, false };
std::valarray<bool> in2 = { true, false, true, false };
std::array<bool, 6> in3 = { true, false, true, false, true, false };
std::vector<bool>   out = { true, false, true, false };

using namespace aaa;

out = logical_not(in1); // Same type of containers.
logical_not(in2, out); // Mixed type of containers.
logical_not(begin(in3) + 1, begin(in3) + 5, begin(out)); // Ranges of iterators.
```

@{
*/

template<typename InputIterator, typename OutputIterator>
void logical_not(InputIterator first_in, InputIterator last_in, OutputIterator first_out)
{
    std::transform(first_in, last_in, first_out, std::logical_not<bool>());
}

template<typename Container1, typename Container2>
void logical_not(const Container1& in, Container2& out)
{
    using namespace std;
    aaa::logical_not(begin(in), end(in), begin(out));
}

template<typename Container>
Container logical_not(const Container& in)
{
    auto out = in;
    aaa::logical_not(in, out);
    return out;
}

/** @} */

} // namespace aaa
