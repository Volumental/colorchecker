#pragma once

#include <set>

namespace komb {

template <typename Container>
auto makeSet(const Container& in) -> std::set<typename Container::value_type>
{
    using namespace std;
    return set<typename Container::value_type>(begin(in), end(in));
}

} // namespace komb
