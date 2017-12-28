#pragma once

#include <string>
#include <vector>

#include <common/Path.hpp>

namespace komb {
namespace text_file {

std::vector<std::string> readLines(const fs::path& filepath);
std::vector<std::string> readWords(const fs::path& filepath);
std::vector<double> readNumbers(const fs::path& filepath);

} // namespace text_file
} // namespace komb
