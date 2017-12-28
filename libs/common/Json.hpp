#pragma once

#include <string>

#include <common/Logging.hpp>
#include <common/Throw.hpp>

#define CONFIGURU_IMPLICIT_CONVERSIONS 1 // Allow this: float f = some_cfg;
#define CONFIGURU_VALUE_SEMANTICS 1 // Each copy should be a deep copy

#define CONFIGURU_ONERROR(message_str)                                                             \
    do                                                                                             \
    {                                                                                              \
        LOG_THROW std::runtime_error(message_str);                                                 \
    } while (false)

#include <configuru.hpp>

namespace komb {

using Json = configuru::Config;

} // namespace komb

// Enable ERROR_CONTEXT("json", &some_json);
namespace loguru {
Text ec_to_text(const komb::Json* json); // NOLINT
} // namespace loguru
