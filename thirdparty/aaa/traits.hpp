#pragma once

#include <type_traits>

namespace aaa {

template<typename Container>
using value_type = typename Container::value_type;

template<typename Iterator>
using value_type_i = typename std::iterator_traits<Iterator>::value_type;

template<typename Container>
using iterator = typename Container::iterator;

template<typename Container>
using const_iterator = typename Container::const_iterator;


template<typename T> struct sqrt_type { using type = double; };
template<>           struct sqrt_type<float> { using type = float; };
template<>           struct sqrt_type<long double> { using type = long double; };
template<typename T> using sqrt_type_t = typename sqrt_type<T>::type;

template<typename Container>
using sqrt_value_type = sqrt_type_t<value_type<Container>>;

template<typename Iterator>
using sqrt_value_type_i = sqrt_type_t<value_type_i<Iterator>>;


// This is what we want. However, it requires Expression SFINAE, which is not
// yet supported in MSVC, unless MSVC uses Clang.
#if !_MSC_VER || __clang__

template<typename A, typename B, typename C>
using check_sum = decltype(C{ A{} + B{} })*;

template<typename A, typename B, typename C>
using check_difference = decltype(C{ A{} - B{} })*;

template<typename A, typename B, typename C>
using check_product = decltype(C{ A{} * B{} })*;

template<typename A, typename B, typename C>
using check_ratio = decltype(C{ A{} / B{} })*;

#else // Fallback for MSVC without Clang.

template<typename A, typename B, typename C = B>
using enable_if_same = typename std::enable_if<
    std::is_same<A, B>::value && std::is_same<B, C>::value, void*>::type;

template<typename A, typename B, typename C>
using check_sum = enable_if_same<A, B, C>;

template<typename A, typename B, typename C>
using check_difference = enable_if_same<A, B, C>;

template<typename A, typename B, typename C>
using check_product = enable_if_same<A, B, C>;

template<typename A, typename B, typename C>
using check_ratio = enable_if_same<A, B, C>;

#endif

} // namespace aaa
