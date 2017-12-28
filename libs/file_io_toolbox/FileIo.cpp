#include "FileIo.hpp"

#include <fstream>
#include <string>
#include <vector>

#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>

#include <common/algorithm/Container.hpp>
#include <common/Debug.hpp>
#include <common/Logging.hpp>
#include <common/ScopeExit.hpp>
#include <common/Throw.hpp>

#include "FileSystem.hpp"

namespace komb {

static const std::vector<std::string> kMeshExtensions = {".bmf", ".ctm", ".obj", ".ply", ".stl"};

bool isMeshExtension(std::string extension)
{
    boost::algorithm::to_lower(extension);
    return contains(kMeshExtensions, extension);
}

std::vector<std::string> readAllLines(const fs::path& path)
{
    std::ifstream input(path.string());
    CHECK(input.good()) << "Could not open " << path;

    std::string line;
    std::vector<std::string> result;
    while (std::getline(input, line))
    {
        result.push_back(line);
    }
    return result;
}

boost::optional<std::string> readTextFile(const fs::path& path)
{
    try
    {
        std::ifstream file(path.c_str());
        if (!file.is_open())
        {
            LOG(WARNING) << "Failed to open file at '" << path.string() << "'";
            return boost::none;
        }

        const auto size = fileSizeBytes(path);
        std::string str(size, '\0');
        file.read(&str[0], size);
        return str;
    }
    catch (std::exception& e)
    {
        LOG_F(ERROR, "Failed to read '%s': %s", path.c_str(), e.what());
        return boost::none;
    }
}

void writeTextFile(const fs::path& path, const std::string& contents)
{
    writeBinaryFile(path, contents.data(), contents.size());
}

void writeBinaryFile(const fs::path& path, const void* data, size_t num_bytes)
{
    auto path_str = path.string();
    FILE* fp = fopen(path_str.c_str(), "wb");
    if (!fp)
    {
        LOG_THROW std::runtime_error("Failed to create '" + path_str + "': " + komb::strerror());
    }
    const auto bytes_written = fwrite(data, 1, num_bytes, fp);
    const bool did_close_ok = fclose(fp) == 0;
    if (bytes_written != num_bytes || !did_close_ok)
    {
        try { fs::remove(path); } catch (...) {} // Clean up
        LOG_THROW std::runtime_error("Failed to write to '" + path_str + "': " + komb::strerror());
    }
}

boost::optional<std::vector<uint8_t>> readBinaryFile(const fs::path& path)
{
    // This can be greatly optimized with fstat and/or mmap
    auto path_str = path.string();
    FILE* fp = fopen(path_str.c_str(), "rb");
    if (fp == nullptr)
    {
        LOG(WARNING) << "Failed to load file at '" << path_str << "'";
        return boost::none;
    }
    SCOPE_EXIT{ fclose(fp); };

    const size_t kChunkSize = 64 * 1024;

    std::vector<uint8_t> bytes;

    for (;;)
    {
        bytes.resize(bytes.size() + kChunkSize);
        auto num_read = fread(bytes.data() + bytes.size() - kChunkSize, 1, kChunkSize, fp);
        if (num_read < kChunkSize)
        {
            bytes.resize(bytes.size() - kChunkSize + num_read);
            break;
        }
    }

    return bytes;
}

std::string readTextFileWithComments(const fs::path& dataset_path)
{
    std::ifstream input(dataset_path.string());
    CHECK(input.good()) << "Could not open " << dataset_path;
    auto line   = std::string{};
    auto result = std::string{};
    while (std::getline(input, line))
    {
        if (line.empty() || line[0] != '#')
        {
            result += line + '\n';
        }
    }
    return result;
}

} // namespace komb
