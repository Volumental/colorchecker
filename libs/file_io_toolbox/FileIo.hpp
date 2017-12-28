#pragma once

#include <sstream>
#include <string>
#include <vector>

#include <boost/optional.hpp>

#include <common/Path.hpp>

namespace komb {

bool isMeshExtension(std::string extension);

/// Read each line into a vector. Empty lines will be
/// included unless it's the last one in the file
std::vector<std::string> readAllLines(const fs::path& path);

boost::optional<std::string> readTextFile(const fs::path& path);
void writeTextFile(const fs::path& path, const std::string& contents);

void writeBinaryFile(const fs::path& path, const void* data, size_t num_bytes);
boost::optional<std::vector<uint8_t>> readBinaryFile(const fs::path& path);

template <typename T>
void writeVectorToTextFile(const fs::path& path, const std::vector<T>& vec)
{
    std::stringstream ss;
    for (const auto& elem : vec)
    {
        ss << elem << std::endl;
    }
    writeTextFile(path, ss.str());
}

// Skip lines starting with #.
std::string readTextFileWithComments(const fs::path& dataset_path);

} // namespace komb
