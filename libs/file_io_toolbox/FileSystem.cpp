#define BOOST_NO_CXX11_SCOPED_ENUMS // Needed to get copy_file to link

#include "FileSystem.hpp"

#include <algorithm>
#include <set>
#include <string>
#include <vector>

#include <sys/stat.h>
#include <sys/types.h>

#include <boost/algorithm/string/predicate.hpp>
#include <boost/filesystem.hpp>

#include <common/algorithm/Container.hpp>
#include <common/Compare.hpp>
#include <common/Debug.hpp>
#include <common/Logging.hpp>
#include <common/String.hpp>
#include <common/Throw.hpp>

namespace komb {

void sortFileNames(std::vector<std::string>& io_filenames)
{
    komb::sort(io_filenames, numberAwareStringLess);
}

void sortPaths(std::vector<fs::path>& io_paths)
{
    komb::sort(io_paths, numberAwarePathLess);
}

std::vector<fs::path> getFilesInDir(const fs::path& directory)
{
    std::vector<fs::path> result;
    fs::directory_iterator pos(directory);
    fs::directory_iterator end;

    for (; pos != end; ++pos)
    {
        if (fs::is_regular_file(pos->status()))
        {
            result.push_back(pos->path());
        }
    }

    sortPaths(result);
    return result;
}

std::vector<std::string> getFilenamesInDir(const fs::path& directory)
{
    std::vector<std::string> result;
    fs::directory_iterator pos(directory);
    fs::directory_iterator end;

    for (; pos != end; ++pos)
    {
        if (fs::is_regular_file(pos->status()))
        {
            result.push_back(pos->path().filename().string());
        }
    }

    sortFileNames(result);
    return result;
}

/// e.g. ".png;.jpg"
std::set<std::string> parseExtension(const std::string& ext)
{
    std::set<std::string> extensions;
    if (ext == "")
    {
        extensions.insert("");
    }
    else
    {
        std::stringstream ss(ext);
        std::string item;
        while (std::getline(ss, item, ';'))
        {
            extensions.insert(item);
        }
    }
    return extensions;
}

std::vector<fs::path> getFilesInDir(
    const fs::path& directory,
    const std::string& ext)
{
    if (directory.empty() || !fs::exists(directory) || !fs::is_directory(directory))
    {
        ABORT_S() << directory <<  ": Not a valid directory!";
    }

    std::vector<fs::path> result;
    fs::directory_iterator pos(directory);
    fs::directory_iterator end;

    const auto extensions = parseExtension(ext);

    for (; pos != end; ++pos)
    {
        if (fs::is_regular_file(pos->status()))
        {
            if (extensions.count(fs::extension(*pos)))
            {
                result.push_back(pos->path());
            }
        }
    }

    sortPaths(result);
    return result;
}

std::vector<fs::path> getSubDirectoryPaths(const fs::path& directory)
{
    std::vector<fs::path> out_directories;

    if (directory.empty() || !fs::exists(directory) || !fs::is_directory(directory))
    {
        ABORT_S() << directory.string() <<  ": Not a valid directory!";
    }
    fs::directory_iterator pos(directory);
    fs::directory_iterator end;
    for (; pos != end; ++pos)
    {
        if (fs::is_directory(pos->status()))
        {
            out_directories.push_back(pos->path());
        }
    }

    sortPaths(out_directories);
    return out_directories;
}

std::vector<std::string> getFilenamesInDir(
    const fs::path& directory,
    const std::string& ext)
{
    if (directory.empty() || !fs::exists(directory) || !fs::is_directory(directory))
    {
        ABORT_S() << directory <<  ": Not a valid directory!";
    }

    std::vector<std::string> result;
    fs::directory_iterator pos(directory);
    fs::directory_iterator end;

    const auto extensions = parseExtension(ext);

    for (; pos != end; ++pos)
    {
        if (fs::is_regular_file(pos->status()))
        {
            if (extensions.count(fs::extension(*pos)))
            {
                result.push_back(pos->path().filename().string());
            }
        }
    }

    sortFileNames(result);
    return result;
}

std::vector<std::string> getSubDirectoryNames(const fs::path& dir)
{
    CHECK_F(fs::is_directory(dir), "Expected '%s' to be a directory", dir.c_str());
    std::vector<std::string> result;
    fs::directory_iterator end_iter;
    for (fs::directory_iterator dir_iter(dir); dir_iter != end_iter; ++dir_iter)
    {
        if (fs::is_directory(dir_iter->status()))
        {
            auto relative_path = relativeTo(dir, dir_iter->path());
            result.push_back(relative_path.string());
        }
    }
    sortFileNames(result);
    return result;
}

void collectPaths(const fs::path& root, std::vector<fs::path>& out_paths)
{
    fs::directory_iterator end_iter;
    for (fs::directory_iterator dir_iter(root); dir_iter != end_iter; ++dir_iter)
    {
        if (fs::is_regular_file(dir_iter->status()))
        {
            out_paths.push_back(dir_iter->path());
        }
        else if (fs::is_directory(dir_iter->status()))
        {
            collectPaths(dir_iter->path(), out_paths);
        }
    }
}

/// Returns to all files in a directory, recursively.
std::vector<fs::path> filePathsRecursive(const fs::path& root)
{
    std::vector<fs::path> results;
    collectPaths(root, results);
    sortPaths(results);
    return results;
}

bool isNewerThan(const fs::path& a, const fs::path& b)
{
    time_t a_time = fs::last_write_time(a);
    time_t b_time = fs::last_write_time(b);
    return difftime(a_time, b_time) > 0;
}

fs::path relativeTo(
    const fs::path& from, const fs::path& to)
{
    // Based on http://stackoverflow.com/a/29221546
    fs::path::const_iterator from_iter = from.begin();
    fs::path::const_iterator to_iter = to.begin();

    while (from_iter != from.end() && to_iter != to.end() && *to_iter == *from_iter)
    {
        ++to_iter;
        ++from_iter;
    }

    fs::path result;

    while (from_iter != from.end())
    {
        if (*from_iter != ".")
        {
            result /= "..";
        }
        ++from_iter;
    }

    while (to_iter != to.end())
    {
        result /= *to_iter;
        ++to_iter;
    }

    return result;
}

fs::path homeDir()
{
#ifdef _WIN32
    auto user_profile = getenv("USERPROFILE");
    CHECK(user_profile != nullptr) << "Missing USERPROFILE";
    return user_profile;
#else // _WIN32
    auto home = getenv("HOME");
    CHECK(home != nullptr) << "Missing HOME";
    return home;
#endif // _WIN32
}

fs::path expandHome(const fs::path& path)
{
    if (path == "~")
    {
        return homeDir();
    }
    else if (boost::starts_with(path.string(), "~/"))
    {
        return homeDir() / path.string().substr(2);
    }
    else
    {
        return path;
    }
}

bool checkCopyInputs(const fs::path& source, const fs::path& destination)
{
    try
    {
        if (!fs::exists(source) || !fs::is_directory(source))
        {
            LOG(ERROR) << "Source directory " << source.string()
                      << " does not exist or is not a directory.";
            return false;
        }
        fs::create_directory(destination);
        return true;
    }
    catch (const fs::filesystem_error& e)
    {
        LOG(ERROR) << e.what();
        return false;
    }
}

bool copyDir(const fs::path& source, const fs::path& destination)
{
    if (!checkCopyInputs(source, destination)) { return false; }
    bool success = true;

    for (fs::directory_iterator it(source); it != fs::directory_iterator(); ++it)
    {
        try
        {
            const auto dest_path = destination / it->path().filename();
            if (fs::is_directory(it->path()))
            {
                success &= copyDir(it->path(), dest_path);
            }
            else
            {
                fs::copy_file(it->path(), dest_path, fs::copy_option::overwrite_if_exists);
            }
        }
        catch (const fs::filesystem_error& e)
        {
            LOG(ERROR) << e.what();
            success = false;
        }
    }
    return success;
}

bool copyDirAsLinks(const fs::path& source, const fs::path& destination)
{
    if (!checkCopyInputs(source, destination)) { return false; }
    bool success = true;

    for (fs::directory_iterator it(source); it != fs::directory_iterator(); ++it)
    {
        try
        {
            const auto dest_path = destination / it->path().filename();
            if (fs::is_directory(it->path()))
            {
                success &= copyDirAsLinks(it->path(), dest_path);
            }
            else
            {
                if (!fs::exists(dest_path))
                {
                    fs::create_symlink(it->path(), dest_path);
                }
            }
        }
        catch (const fs::filesystem_error& e)
        {
            LOG(ERROR) << e.what();
            success = false;
        }
    }
    return success;
}

bool copyFile(const fs::path& source, const fs::path& destination)
{
    try
    {
        fs::copy_file(source, destination, fs::copy_option::overwrite_if_exists);
    }
    catch (const fs::filesystem_error& e)
    {
        LOG(ERROR) << e.what();
        return false;
    }
    return true;
}

bool isHidden(const fs::path& path)
{
    return boost::starts_with(path.filename().string(), ".");
}

size_t fileSizeBytes(const fs::path& path)
{
    const uintmax_t num_bytes = fs::file_size(path);
    if (num_bytes == static_cast<uintmax_t>(-1))
    {
        LOG_THROW std::runtime_error("Failed to get file size of '" + path.string() + "'");
    }
    return static_cast<size_t>(num_bytes);
}

time_t lastAccessedTime(const fs::path& path)
{
    struct stat info;
    if (stat(path.c_str(), &info) != 0)
    {
        LOG_THROW std::runtime_error("Failed to stat '" + path.string() + "': " + komb::strerror());
    }
    return info.st_atime;
}

bool hasWritePermission(const fs::path& path)
{
    struct stat info;
    if (stat(path.c_str(), &info) != 0)
    {
        LOG_THROW std::runtime_error("Failed to stat '" + path.string() + "': " + komb::strerror());
    }
    return (info.st_mode & S_IWUSR) != 0;
}

void setWritePermission(const fs::path& path, bool allow_writing) //NOLINT
{
    struct stat info;
    if (stat(path.c_str(), &info) != 0)
    {
        LOG_THROW std::runtime_error("Failed to stat '" + path.string() + "': " + komb::strerror());
    }

    auto mode = info.st_mode;

    if (allow_writing)
    {
        mode |= S_IWUSR;
    }
    else
    {
        mode &= ~S_IWUSR;
    }

    CHECK_EQ(chmod(path.c_str(), mode), 0) << "Failed to change permissions for '"
        << path.c_str() << "': " << komb::strerror();
}

bool hasExtension(const fs::path& filepath, const std::string& extension)
{
    return boost::ends_with(filepath.string(), "." + extension);
}

bool isObj(const fs::path& filepath)
{
    return hasExtension(filepath, "obj");
}

bool isStl(const fs::path& filepath)
{
    return hasExtension(filepath, "stl");
}

bool isJson(const fs::path& filepath)
{
    return hasExtension(filepath, "json");
}

bool removeDirectory(const fs::path& dir)
{
    boost::system::error_code err;
    fs::remove_all(dir, err);
    if (err)
    {
        LOG(ERROR) << "Could not delete " << dir << ": " << err.message();
        return false;
    }

    return true;
}

bool createDirectories(const fs::path& dir)
{
    boost::system::error_code err;
    fs::create_directories(dir, err);
    if (err)
    {
        LOG(ERROR) << "Could not create " << dir << ": " << err.message();
        return false;
    }

    return true;
}

void createParentPath(const fs::path& file_path)
{
    if (file_path.has_parent_path())
    {
        const fs::path parent_path = file_path.parent_path();

        boost::system::error_code err;
        fs::create_directories(parent_path, err);

        if (err)
        {
            const std::string err_msg = err.message();
            ABORT_F("Could not create '%s': %s", parent_path.c_str(), err_msg.c_str());
        }
    }
}

}  // namespace komb
