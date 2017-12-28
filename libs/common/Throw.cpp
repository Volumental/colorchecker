#include "Throw.hpp"

#include <string>

#include "Logging.hpp"
#include "String.hpp"

namespace komb {
namespace internal {

void ExceptionLogger::logException(const std::exception& e) const
{
    std::string full_message = stacktraceAndErrorContext(2);
    full_message += komb::strprintf("%s:%u: %s", file_, line_, current_function_) + "\n";
    full_message += std::string{"LOG_THROW: "} + e.what() + "\n";
    full_message += "-----------------------------------------------------------";
    loguru::log(loguru::Verbosity_ERROR, file_, line_, "Exception thrown: %s\n%s",
                e.what(), full_message.c_str());
}

std::string logAndFormatExceptionText(
    const char* test, const char* file, unsigned int line, const char* current_function,
    const char* format, ...)
{
    va_list vlist;
    va_start(vlist, format);
    std::string what = strprintfv(format, vlist);
    va_end(vlist);

    // Log right away in case the exception is never caught:
    std::string full_message = stacktraceAndErrorContext(2);
    full_message += komb::strprintf("%s:%u: %s", file, line, current_function) + "\n";
    full_message += test;
    full_message += "\n-----------------------------------------------------------";
    loguru::log(loguru::Verbosity_ERROR, file, line, "%s\n%s", what.c_str(), full_message.c_str());

    // Just include the user-supplied text in the actual what:
    return what;
}

}  // namespace internal
}  // namespace komb
