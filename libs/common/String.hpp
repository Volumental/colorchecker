#pragma once

#include <cstdarg>
#include <cstring>
#include <string>
#include <vector>

namespace komb {

// strprintf acts like printf, but writes to a std::string.

// Try to catch format string errors at compile time with compiler-specific extensions:
#ifdef _MSC_VER
    void strprintfvAppend(std::string& destination,
        _In_z_ _Printf_format_string_ const char* format, va_list);
    std::string strprintfv(_In_z_ _Printf_format_string_ const char* format, va_list);
    std::string strprintf(_In_z_ _Printf_format_string_ const char* format, ...);
#elif defined(__clang__) || defined(__GNUC__)
    void strprintfvAppend(std::string& io_destination,
        const char* format, va_list) __attribute__((format(printf, 2, 0)));
    std::string strprintfv(const char* format, va_list) __attribute__((format(printf, 1, 0)));
    std::string strprintf(const char* format, ...) __attribute__((format(printf, 1, 2)));
#else
    void strprintfvAppend(std::string& destination, const char* format, va_list);
    std::string strprintfv(const char* format, va_list);
    std::string strprintf(const char* format, ...);
#endif

struct StringStream
{
#ifdef _MSC_VER
    void printf(_In_z_ _Printf_format_string_ const char* format, ...);
#elif defined(__clang__) || defined(__GNUC__)
    void printf(const char* format, ...) __attribute__((format(printf, 2, 3)));
#else
    void printf(const char* format, ...);
#endif

    inline void write(const void* data, size_t num_bytes)
    {
        auto old_size = string.size();
        string.resize(string.size() + num_bytes);
        memcpy(&string[old_size], data, num_bytes);
    }

    std::string string;
};

/// Turn "HelloWorld", "Hello world", "Hello World" etc into "hello_world".
std::string snakeCased(const std::string& str);

/// Splits on each \n.
std::vector<std::string> splitLinesKeepEmpty(const std::string& str);

/// Splits input into *non-empty* lines.
std::vector<std::string> splitLinesStripEmpty(const std::string& str);

/// splitSpaces("a  b\r\n")  =>  {"a", "b"}
std::vector<std::string> splitSpaces(const std::string& str);
void splitSpaces(const std::string& str, std::vector<std::string>& out_tokens);

/// splitSlashes("a//b")  =>  {"a", "", "b"}
void splitSlashes(const std::string& str, std::vector<std::string>& out_tokens);

/// splitCommas("a,,b") => {"a", "", "b"}
void splitCommas(const std::string& str, std::vector<std::string>& out_tokens);
inline std::vector<std::string> splitCommas(const std::string& str)
{
    std::vector<std::string> out;
    splitCommas(str, out);
    return out;
}

/// splitDelimiter("a,,b") => {"a", "", "b"}
void splitDelimiter(const std::string& str, const char delimiter,
    std::vector<std::string>& out_tokens);

bool beginsWith(const std::string& haystack, const std::string& needle);

/// Return the filename part of a path, e.g. whatever comes after the last / or \, if any.
const char* filename(const char* path);

/// Add `indentation` to the start of each new line. Also removes all *trailing* whitespace.
std::string indent(std::string str, const std::string& indentation = "    ");

/// Do "German" formatting of float values with comma instead of decimal point.
std::string germanFormat(float value);

/// Keep a character iff predicate(element) == true.
template <typename Predicate>
auto filter(const std::string& input_str, const Predicate& predicate) -> std::string
{
    std::string output_str;
    for (const char c : input_str)
    {
        if (predicate(c))
        {
            output_str += c;
        }
    }
    return output_str;
}

/// Dies on bad input.
int parseInt(const std::string& str);

/// Dies on bad input.
float parseFloat(const std::string& str);

} // namespace komb
