#include "Debug.hpp"

#include <string>

#include "Logging.hpp"

namespace komb {

std::string strerror()
{
    const auto text = loguru::errno_as_text();
    return text.c_str();
}

} // namespace komb
