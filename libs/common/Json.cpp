#include "Json.hpp"

#include <sstream>
#include <string>

#define CONFIGURU_IMPLEMENTATION 1
#include <configuru.hpp>

namespace loguru {

Text ec_to_text(const komb::Json* json) // NOLINT
{
    CHECK_NOTNULL(json);
    std::stringstream ss;
    ss << *json;
    std::string str = ss.str();
    return Text{strdup(str.c_str())};
}

} // namespace loguru
