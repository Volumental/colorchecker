#pragma once

#include <array>
#include <string>
#include <vector>

/// For logging image::dimensions when using eg. CHECK_EQ.
/// Needs to be declared before loguru to avoid mac build problems,
/// "operator<<' should be declared prior to the call site".
template<typename T, size_t N>
std::ostream& operator<<(std::ostream& stream, const std::array<T, N>& array)
{
    for (size_t i = 0; i < N; ++i)
    {
        if (i + 1 < N)
        {
            stream << array[i] << " ";
        }
        else
        {
            stream << array[i];
        }
    }
    return stream;
}

template<typename T>
std::ostream& operator<<(std::ostream& stream, const std::vector<T>& vector)
{
    for (size_t i = 0; i < vector.size(); ++i)
    {
        if (i + 1 < vector.size())
        {
            stream << vector[i] << " ";
        }
        else
        {
            stream << vector[i];
        }
    }
    return stream;
}

#define LOGURU_FILENAME_WIDTH 25
#define LOGURU_REPLACE_GLOG 1
#define LOGURU_REDEFINE_ASSERT 1

#include <loguru.hpp>

namespace komb {

/// Name the current thread for logging and profiling. Prefer snake_case_names
/// Try to limit the thread name to 15 characters or less to ensure they fully fit in the logs.
void setThreadName(const std::string& name);

/// @param skip specifies how many extra stack frames to skip.
std::string stacktraceAndErrorContext(int skip);

} // namespace komb
