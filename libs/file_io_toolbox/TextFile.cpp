#include "TextFile.hpp"

#include <fstream>
#include <string>
#include <vector>

#include <boost/filesystem.hpp>

#include <common/algorithm/Vector.hpp>

namespace komb {
namespace text_file {

std::vector<std::string> readLines(const fs::path& filepath)
{
    CHECK_F(fs::exists(filepath), "Failed to find '%s'", filepath.c_str());
    using namespace std;
    ifstream is(filepath.c_str());
    auto line = string();
    auto lines = vector<string>();
    while (getline(is, line))
    {
        lines.push_back(line);
    }
    return lines;
}

std::vector<std::string> readWords(const fs::path& filepath)
{
    CHECK_F(fs::exists(filepath), "Failed to find '%s'", filepath.c_str());
    using namespace std;
    ifstream is(filepath.c_str());
    const auto start = istream_iterator<string>(is);
    const auto end   = istream_iterator<string>();
    return vector<string>(start, end);
}

std::vector<double> readNumbers(const fs::path& filepath)
{
    const auto words = readWords(filepath);
    const auto convert_possibly_nan_number = [](std::string s) -> double
    {
        return std::stod(s);
    };
    return map(words, convert_possibly_nan_number);
}

} // namespace text_file
} // namespace komb
