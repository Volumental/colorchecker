/** \mainpage

# Contents
- Overview
- Requirements
- Design and Rationale
- Building
- Examples

# Overview

This library consists of a set of templated functions for doing basic arithmetic
operations on containers and ranges of iterators. The elements of the
ranges/containers can be any built in aritmetic type, e.g.
`float`, `double`, `int`, or any other type that supports the arithmetic
operations: +, -, *, /. We refer to a range/container of arithmetic elements as
a mathematical vector. This library defines vector operations for arbitrary
finite vector spaces. This library does not define any specific data structure
for the vectors, but operates on general vectors/ranges/containers.

The library consists of different modules:
- @ref vector_space.
  This module defines elementwise arithmetic operations on vectors.
  It consists of the functions:
  @ref add, @ref subtract, @ref negate, @ref multiply, @ref divide.
- @ref norms_metrics.
  This module defines norms/lengths and metrics/distances for vectors.
  These functions take one or two vectors and returns a single scalar.
  It consists of the functions:
  `dot`, `norm`, `distance`, `squared_norm`, `squared_distance`.
  They are defined for the following vector spaces:
  @ref euclidean_space, @ref manhattan_space, @ref maximum_space.
- @ref misc_algorithms. This contains the functions:
  `sum`, `convert`.
- @ref logical.
  This module defines elementwise boolean operations on ranges/containers.
  The elements should be of type `bool`,
  or any other type that supports the the boolean operations &&, ||, !.
  The module contains the functions:
  @ref logical_and, @ref logical_or, @ref logical_not.
- @ref std_algorithms_container.
  This module defines container versions of some range
  algorithms from the standard library header
  [algorithm](http://www.cplusplus.com/reference/algorithm/).
  It only does it for the algorithms that are used a lot for arithmetic types.
  It contains the functions:
  `fill`, `copy`, `min_element`, `max_element`, `minmax_element`.

# Requirements

All the functions of this library assume that the input and output is:
- Either ranges of iterators, or standard like containers, i.e. they have begin
  and end functions that give iterators.
- The elements of the ranges/containers should support arithmetic operations:
  +, -, *, /. The functions in the module *Logical Operations* is the only
  exception to this. It assumes that the elements support the boolean operations
  &&, ||, !.
- The functions in the norms and metrics module assume that the default
  construction of a scalar gives zero, which is true for the built-in arithmetic
  types. The function `sum` also assumes this.

# Design and Rationale

This library deals with arithmetic operations on mathematical vectors,
represented as ranges or containers.
It is designed for vectors in arbitrary dimensions. It is NOT optimized for small
vectors, which you typically use to represent geometry in 2D and 3D. For that
use case it might be more optimal to use other libraries like:
[GLM](http://glm.g-truc.net/) and [Eigen](http://eigen.tuxfamily.org/). 

C++ libraries that deals with mathematical vectors in arbitrary dimensions
can usually be divided into two categories:
-# Those that implement a vector class and overload its arithmetic operators.
   Examples of this are:
   [std::valarray](http://www.cplusplus.com/reference/valarray/),
   [Eigen](http://eigen.tuxfamily.org/),
   [Blitz++](https://sourceforge.net/projects/blitz/),
   [OpenCV](http://opencv.org/).
-# Those that implement general algorithms that work on any container or
   range of iterators. An example of this is the standard library and the headers
   [algorithm](http://www.cplusplus.com/reference/algorithm/)
   and [numeric](http://www.cplusplus.com/reference/numeric/).

This library follows the second approach. It implements some common arithmetic
algorithms that are missing in the standard library.

The two approaches have different advantages and disadvantages:
-# The advantage of the first approach is that the syntax can be very concise,
   when you overload the arithmetic operators to act directly on the vector class.
   This approach can also have performance advantages, since it couples the
   algorithm to the data structure. It is then possible to use techniques
   like *expression templates* or *aligned memory* and *vectorization*.
-# The advantage of the second approach is that it allows *generic programming*.
   Code that uses such libraries is more general since it is less coupled to a
   specific data structure. This is good for readability and reuse of the code.

# Building

The library is header only, which makes it easy to use and build. Download it
and add it to the include directory of your project and `#include <aaa.hpp>`.

# Examples

The algorithms can be used on arbitrary containers like this:
```
// Blend two images.
Image blend(const Image& in1, const Image& in2)
{
    using namespace aaa;
    auto a = add(in1, in2); // Add the images together elementwise.
    return divide(a, 2); // Divide the result elementwise with 2 to get the mean image.
}

// Returns the projection of a on b.
std::vector<float> project(const std::vector<float>& a, const std::vector<float>& b)
{
    using namespace aaa;
    using namespace aaa::euclidean;
    // First we compute the scaling factor of the projection by taking the dot
    // product of the vectors. We normalize with the squared_norm of the vector
    // that we are projecting on.
    const auto scaling = dot(a, b) / squared_norm(b);
    // The projection of a on b is like b,
    // but multiplied elementwise with the scaling factor.
    return multiply(scaling, b);
}
```
*/

/**
@defgroup vector_space Vector Spaces
@{
@defgroup add add
@defgroup subtract subtract
@defgroup multiply multiply
@defgroup divide divide
@defgroup negate negate
@}

@defgroup norms_metrics Normed and Metric Spaces
@{
@defgroup euclidean_space Euclidean Space (L-2)
@defgroup manhattan_space Manhattan Space (L-1)
@defgroup maximum_space Maximum Space (L-Infinity)
@}

@defgroup logical Logical Operations
@{
@defgroup logical_and logical_and
@defgroup logical_or logical_or
@defgroup logical_not logical_not
@}

@defgroup misc_algorithms Misc Operations

@defgroup std_algorithms_container STD Algorithms on Containers

*/


// TODO: should the elements be passed by value or const reference?

#pragma once

#include "std_algorithms_container.hpp"

#include "misc_algorithms.hpp"

#include "add.hpp"
#include "subtract.hpp"
#include "multiply.hpp"
#include "divide.hpp"
#include "negate.hpp"

#include "euclidean_space.hpp"
#include "manhattan_space.hpp"
#include "maximum_space.hpp"

#include "logical_and.hpp"
#include "logical_or.hpp"
#include "logical_not.hpp"
