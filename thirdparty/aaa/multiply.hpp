#pragma once

#include <algorithm>

#include "traits.hpp"

namespace aaa {

/**
@addtogroup multiply

Examples:
```
std::vector<double>   in1 = { 1.1, 2.2, 3.3, 4.4, 5.5 };
std::vector<double>   in2 = { 1.1, 2.2, 3.3, 4.4, 5.5 };
std::valarray<double> in3 = { 1.1, 2.2, 3.3, 4.4, 5.5 };
std::array<double, 5> in4 = { 1.1, 2.2, 3.3, 4.4, 5.5 };
std::array<double, 9> in5 = { 1.1, 2.2, 3.3, 4.4, 5.5, 6.6, 7.7, 8.8, 9.9 };
std::vector<double>   out = { 1.1, 2.2, 3.3, 4.4, 5.5 };

using namespace aaa;

// In these three examples we want the same type of container
// for both the input and output. We can then return the output by value.
out = multiply(in1, in2); // Elementwise multiplication of two vectors.
out = multiply(2.5, in2); // Elementwise multiplication of a scalar and a vector.
out = multiply(in1, 3.3); // Elementwise multiplication of a vector and a scalar.

// In these three examples we mix arbitrary type of containers
// for the input and output. We then use a reference for output.
multiply(in3, in4, out); // Elementwise multiplication of two vectors.
multiply(4.9, in4, out); // Elementwise multiplication of a scalar and a vector.
multiply(in3, 2.1, out); // Elementwise multiplication of a vector and a scalar.

// In these three examples we use iterators for the input and output.
multiply(begin(in5) + 1, begin(in5) + 6, begin(in4), begin(out)); // Elementwise multiplication of two vectors.
multiply(4.9, begin(in5) + 1, begin(in5) + 6, begin(out)); // Elementwise multiplication of a scalar and a vector.
multiply(begin(in5) + 1, begin(in5) + 6, 2.1, begin(out)); // Elementwise multiplication of a vector and a scalar.
```

@{
*/

////////////////////////////////////////////////////////////////////////////////
// iterators

template<typename InputIterator1, typename InputIterator2, typename OutputIterator,
    check_product<value_type_i<InputIterator1>, value_type_i<InputIterator2>, value_type_i<OutputIterator>> = nullptr>
void multiply(InputIterator1 first_left, InputIterator1 last_left, InputIterator2 first_right, OutputIterator first_out)
{
    auto f = [](const value_type_i<InputIterator1>& left, const value_type_i<InputIterator2>& right)
    {
        return left * right;
    };
    std::transform(first_left, last_left, first_right, first_out, f);
}

template<typename Element, typename InputIterator, typename OutputIterator,
    check_product<Element, value_type_i<InputIterator>, value_type_i<OutputIterator>> = nullptr>
void multiply(const Element& left, InputIterator first_right, InputIterator last_right, OutputIterator first_out)
{
    auto f = [&](const value_type_i<InputIterator>& right) { return left * right; };
    std::transform(first_right, last_right, first_out, f);
}

template<typename InputIterator, typename Element, typename OutputIterator,
    check_product<value_type_i<InputIterator>, Element, value_type_i<OutputIterator>> = nullptr>
void multiply(InputIterator first_left, InputIterator last_left, const Element& right, OutputIterator first_out)
{
    auto f = [&](const value_type_i<InputIterator>& left) { return left * right; };
    std::transform(first_left, last_left, first_out, f);
}

////////////////////////////////////////////////////////////////////////////////
// containers

template<typename Container1, typename Container2, typename Container3,
    check_product<value_type<Container1>, value_type<Container2>, value_type<Container3>> = nullptr>
    void multiply(const Container1& left, const Container2& right, Container3& out)
{
    assert(left.size() == out.size());
    assert(right.size() == out.size());
    using std::begin;
    using std::end;
    multiply(begin(left), end(left), begin(right), begin(out));
}

template<typename Element, typename Container1, typename Container2,
    check_product<Element, value_type<Container1>, value_type<Container2>> = nullptr>
    void multiply(const Element& left, const Container1& right, Container2& out)
{
    assert(right.size() == out.size());
    using std::begin;
    using std::end;
    multiply(left, begin(right), end(right), begin(out));
}

template<typename Container1, typename Element, typename Container2,
    check_product<value_type<Container1>, Element, value_type<Container2>> = nullptr>
    void multiply(const Container1& left, const Element& right, Container2& out)
{
    assert(left.size() == out.size());
    using std::begin;
    using std::end;
    multiply(begin(left), end(left), right, begin(out));
}

////////////////////////////////////////////////////////////////////////////////
// make container

template<typename Container>
Container multiply(const Container& left, const Container& right)
{
    auto out = left;
    multiply(left, right, out);
    return out;
}

template<typename Container>
Container multiply(const Container& left, const value_type<Container>& right)
{
    auto out = left;
    multiply(left, right, out);
    return out;
}

template<typename Container>
Container multiply(const value_type<Container>& left, const Container& right)
{
    auto out = right;
    multiply(left, right, out);
    return out;
}

/** @} */

} // namespace aaa
