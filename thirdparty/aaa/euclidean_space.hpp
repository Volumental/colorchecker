#pragma once

#include <numeric>

#include "traits.hpp"

namespace aaa {
namespace euclidean {

/**
@addtogroup euclidean_space Euclidean Space

Euclidean space is also known as L-2 space. It defines the the following functions:
- The dot product is also known as the scalar product or Euclidean inner product.
  The dot product of the vectors a and b is defined as:
  \f$ a \cdot b = \sum_i a_i b_i \f$
- The Euclidean norm or length of a vector is defined as:
  \f$ \| a \|_2 = \sqrt{a \cdot a} = \sqrt{\sum_i a_i^2} \f$
- The Euclidean distance between the vectors a and b is defined as:
  \f$ d_2(a, b) = \|a-b\|_2 = \sqrt{\sum_i (a_i-b_i)^2} \f$

We represent mathematical vectors as either containers, or ranges of iterators.
The functions in this module take one or two vectors as input and output a
single scalar. This is sometimes refered to as a reduction or fold operation.
The functions in this module assume that the default construction of a scalar
gives zero, which is true for the built-in arithmetic types.

@{
*/

template<typename InputIterator1, typename InputIterator2>
value_type_i<InputIterator1>
dot(InputIterator1 first_left, InputIterator1 last_left, InputIterator2 first_right)
{
    using value_type1 = const value_type_i<InputIterator1>;
    using value_type2 = const value_type_i<InputIterator2>;
    static_assert(std::is_same<value_type1, value_type2>::value, "Different value types");

    const auto zero = value_type1();
    return std::inner_product(first_left, last_left, first_right, zero);
}

/** The dot product of two vectors.
Each vector is represented by a container.
The two containers should have the same size and value type.
*/
template<typename Container1, typename Container2>
value_type<Container1> dot(const Container1& a, Container2& b)
{
    assert(a.size() == b.size());
    using std::begin;
    using std::end;
    return dot(begin(a), end(a), begin(b));
}

template<typename InputIterator>
value_type_i<InputIterator> squared_norm(InputIterator first, InputIterator last)
{
    return dot(first, last, first);
}

/** The squared Euclidean norm of a vector.
The vector is represented by an arbitrary container.
*/
template<typename Container>
value_type<Container> squared_norm(const Container& a)
{
    using std::begin;
    using std::end;
    return squared_norm(begin(a), end(a));
}

template<typename InputIterator>
sqrt_value_type_i<InputIterator> norm(InputIterator first, InputIterator last)
{
    return sqrt(squared_norm(first, last));
}

/** The Euclidean norm of a vector.
The vector is represented by an arbitrary container.
Returns a floating point type, i.e. it follows the same convention as `std::sqrt`.
*/
template<typename Container>
sqrt_value_type<Container> norm(const Container& a)
{
    using std::begin;
    using std::end;
    return norm(begin(a), end(a));
}


template<typename InputIterator1, typename InputIterator2>
value_type_i<InputIterator1>
squared_distance(InputIterator1 first_left, InputIterator1 last_left, InputIterator2 first_right)
{
    using value_type_left = const value_type_i<InputIterator1>;
    using value_type_right = const value_type_i<InputIterator2>;
    using value_type = value_type_left;
    static_assert(std::is_same<value_type_left, value_type_right>::value, "Different value types");

    const auto zero = value_type();
    auto op1 = [](const value_type& left, const value_type& right)
    {
        return left + right;
    };
    auto op2 = [](const value_type& left, const value_type& right)
    {
        return (left - right) * (left - right);
    };
    return std::inner_product(first_left, last_left, first_right, zero, op1, op2);
}

/** The squared Euclidean distance of two vectors.
Each vector is represented by a container.
The two containers should have the same size and value type.
*/
template<typename Container1, typename Container2>
value_type<Container1> squared_distance(const Container1& left, const Container2& right)
{
    assert(left.size() == right.size());
    using std::begin;
    using std::end;
    return squared_distance(begin(left), end(left), begin(right));
}

template<typename InputIterator1, typename InputIterator2>
sqrt_value_type_i<InputIterator1>
distance(InputIterator1 first_left, InputIterator1 last_left, InputIterator2 first_right)
{
    return sqrt(squared_distance(first_left, last_left, first_right));
}

/** The Euclidean distance of two vectors.
Each vector is represented by a container.
The two containers should have the same size and value type.
Returns a floating point type, i.e. it follows the same convention as `std::sqrt`.
*/
template<typename Container1, typename Container2>
sqrt_value_type<Container1> distance(const Container1& left, const Container2& right)
{
    assert(left.size() == right.size());
    using std::begin;
    using std::end;
    return distance(begin(left), end(left), begin(right));
}

/** @} */

} // namespace euclidean
} // namespace aaa
