#pragma once

#include <string>

#include <boost/current_function.hpp>

#include "Logging.hpp"

namespace komb {
namespace internal {

class ExceptionLogger
{
public:
    explicit ExceptionLogger(const char* file, unsigned line, const char* current_function)
        : file_(file), line_(line), current_function_(current_function) {}

    template<typename E>
    const E& operator%(const E& e) const
    {
        logException(e);
        return e;
    }

private:
    void logException(const std::exception& e) const;

    const char* file_;
    unsigned    line_;
    const char* current_function_;
};

std::string logAndFormatExceptionText(
    const char* test, const char* file, unsigned int line, const char* current_function,
    LOGURU_FORMAT_STRING_TYPE format, ...) LOGURU_PRINTF_LIKE(5, 6);

} // namespace internal
} // namespace komb

/// Acts like a normal throw but logs where the exception was thrown from
#define LOG_THROW \
    throw komb::internal::ExceptionLogger(__FILE__, __LINE__, BOOST_CURRENT_FUNCTION) %

/// Helper for quick-and-easy tests.
///     THROW_IF_F(error_code != 0, std::runtime_error, "Error code: %d", error_code);
#define THROW_IF_F(test, exception_type, message_format, ...)                                      \
    ((test) == false) ? (void)0 : throw exception_type(komb::internal::logAndFormatExceptionText(  \
        "THROW_IF_F is throwing a " #exception_type " because: " #test,                            \
        __FILE__, __LINE__, BOOST_CURRENT_FUNCTION,                                                \
        message_format, ##__VA_ARGS__))
