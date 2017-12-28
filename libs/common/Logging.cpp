#include "Logging.hpp"

#include <string>

#include "Profiler.hpp"

namespace komb {

/// Name the current thread for logging and profiling.
void setThreadName(const std::string& name)
{
    loguru::set_thread_name(name.c_str());
    rmt_SetCurrentThreadName(name.c_str());
}

std::string stacktraceAndErrorContext(int skip)
{
    auto stacktrace    = loguru::stacktrace(skip + 1);
    auto error_context = loguru::get_error_context();

    std::string result;

    result += "-----------------------------------------------------------\n";
    result += std::string{stacktrace.c_str()} + "\n";

    if (error_context.empty())
    {
        result += "-----------------------------------------------------------\n";
    }
    else
    {
        result += error_context.c_str();
        result += "\n";
    }

    return result;
}

} // namespace komb
