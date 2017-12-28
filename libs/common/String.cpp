#include "String.hpp"

#include <cstdarg>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

#include <boost/algorithm/string.hpp>

#include <common/algorithm/Container.hpp>
#include <common/Logging.hpp>
#include <common/Throw.hpp>

namespace komb {

void strprintfvAppend(std::string& io_destination, const char* format, va_list vlist)
{
#ifdef _MSC_VER
    int bytes_needed = vsnprintf(nullptr, 0, format, vlist);
    CHECK_GE(bytes_needed, 0) << "Bad format: '" << format << "'";
    size_t old_size = io_destination.size();
    io_destination.resize(old_size + bytes_needed + 1);
    vsnprintf(&io_destination[old_size], bytes_needed + 1, format, vlist);
    io_destination.resize(io_destination.size() - 1); // Remove final '\0'
#else
    char* buff = nullptr;
    int ret = vasprintf(&buff, format, vlist);
    CHECK_GE(ret, 0) << "Bad format: '" << format << "'";
    io_destination += buff;
    free(buff);
#endif
}

std::string strprintfv(const char* format, va_list vlist)
{
    std::string result;
    strprintfvAppend(result, format, vlist);
    return result;
}

std::string strprintf(const char* format, ...)
{
    va_list vlist;
    va_start(vlist, format);
    std::string result;
    strprintfvAppend(result, format, vlist);
    va_end(vlist);
    return result;
}

void StringStream::printf(const char* format, ...)
{
    va_list vlist;
    va_start(vlist, format);
    strprintfvAppend(this->string, format, vlist);
    va_end(vlist);
}

std::string snakeCased(const std::string& str)
{
    std::stringstream ss;
    auto prev_underscore = true;
    for (auto c : str)
    {
        if (c != (c & 127))
        {
            LOG_THROW std::domain_error("Expected ASCII, got `" + str + "`");
        }
        auto low = static_cast<char>(tolower(c));
        if (low == ' ')
        {
            low = '_';
        }
        else if (low != c && !prev_underscore)
        {
            ss << "_";
        }
        ss << low;
        prev_underscore = (low == '_');
    }
    return ss.str();
}

std::vector<std::string> splitLinesKeepEmpty(const std::string& str)
{
    using namespace boost::algorithm;
    std::vector<std::string> raw_lines;
    split(raw_lines, str, [](char c){ return c == '\n'; });
    return raw_lines;
}

std::vector<std::string> splitLinesStripEmpty(const std::string& str)
{
    using namespace boost::algorithm;
    std::vector<std::string> raw_lines;
    split(raw_lines, str, [](char c){ return c == '\n'; }, token_compress_on);

    std::vector<std::string> non_empty_lines;
    for (const auto& line : raw_lines)
    {
        if (line != "")
        {
            non_empty_lines.push_back(line);
        }
    }

    return non_empty_lines;
}

void splitSlashes(const std::string& str, std::vector<std::string>& out_tokens)
{
    const char delimiter = '/';
    splitDelimiter(str, delimiter, out_tokens);
}

void splitCommas(const std::string& str, std::vector<std::string>& out_tokens)
{
    const char delimiter = ',';
    splitDelimiter(str, delimiter, out_tokens);
}

void splitDelimiter(const std::string& str, const char delimiter,
    std::vector<std::string>& out_tokens)
{
    size_t begin = 0;
    size_t end   = str.find_first_of(delimiter);

    while (end != std::string::npos)
    {
        out_tokens.push_back(str.substr(begin, end - begin));
        begin = ++end;
        end = str.find(delimiter, end);
    }
    out_tokens.push_back(str.substr(begin));
}

std::vector<std::string> splitSpaces(const std::string& str)
{
    std::vector<std::string> tokens;
    splitSpaces(str, tokens);
    return tokens;
}

void splitSpaces(const std::string& str, std::vector<std::string>& out_tokens)
{
    const char* delimiter = " \r\n\t";
    size_t begin = str.find_first_not_of(delimiter);
    size_t end   = str.find_first_of(delimiter, begin);

    while (begin != std::string::npos)
    {
        out_tokens.push_back(str.substr(begin, end - begin));
        begin = str.find_first_not_of(delimiter, end);
        end   = str.find_first_of(delimiter, begin);
    }
}

bool beginsWith(const std::string& haystack, const std::string& needle)
{
    return haystack.compare(0, needle.length(), needle) == 0;
}

const char* filename(const char* path)
{
    const char* result = path;
    for (; *path; ++path)
    {
        if (*path == '/' || *path == '\\')
        {
            result = path + 1;
        }
    }
    return result;
}

std::string indent(std::string str, const std::string& indentation)
{
    boost::replace_all(str, "\n", "\n" + indentation);
    boost::algorithm::trim_right(str);
    str = indentation + str;
    return str;
}

std::string germanFormat(float value)
{
    auto str = strprintf("%g", value);
    replace(str, '.', ',');
    return str;
}

int parseInt(const std::string& str)
{
    try
    {
        size_t index;
        auto result = std::stoi(str, &index);
        CHECK_EQ_F(index, str.size(), "Invalid integer: '%s'", str.c_str());
        return result;
    }
    catch (std::invalid_argument&)
    {
        ABORT_F("Invalid integer: '%s'", str.c_str());
    }
}

float parseFloat(const std::string& str)
{
    try
    {
        size_t index;
        auto result = std::stof(str, &index);
        CHECK_EQ_F(index, str.size(), "Invalid float: '%s'", str.c_str());
        return result;
    }
    catch (std::invalid_argument&)
    {
        ABORT_F("Invalid float: '%s'", str.c_str());
    }
}

} // namespace komb
