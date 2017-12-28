#pragma once

#include <string>

#include "Path.hpp"

namespace komb {

/// like strcmp, but when finding strings of digits, will order them naturally.
/// E.g, "foo_9.png" will come before "foo_10.png".
int numberAwareStrcmp(const char* a, const char* b);

inline bool numberAwareStringLess(const std::string& a, const std::string& b)
{
    return numberAwareStrcmp(a.c_str(), b.c_str()) < 0;
}

inline bool numberAwarePathLess(const fs::path& a, const fs::path& b)
{
    return numberAwareStrcmp(a.c_str(), b.c_str()) < 0;
}

} // namespace komb
