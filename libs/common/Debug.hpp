#pragma once

#include <string>

namespace komb {

/// Like strerror(errno), but thread-safe.
std::string strerror();

} // namespace komb
