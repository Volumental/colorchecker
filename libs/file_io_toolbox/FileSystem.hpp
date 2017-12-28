#pragma once

#include <functional>
#include <string>
#include <vector>

#include <common/Path.hpp>

namespace komb {

/// Sort paths so that "foo_2.png" comes before "foo_10.png".
void sortPaths(std::vector<fs::path>& io_paths);

std::vector<fs::path> getSubDirectoryPaths(const fs::path& directory);

std::vector<fs::path> getFilesInDir(const fs::path& directory);
std::vector<std::string> getFilenamesInDir(const fs::path& directory);

/// Find paths matching one or several extensions.
/// @param directory: Which directory to search in.
/// @param ext: Extensions to look for, e.g. ".png;.jpg"
std::vector<fs::path> getFilesInDir(const fs::path& directory, const std::string& ext);

/// Find filenames matching one or several extensions.
/// @param directory: Which directory to search in.
/// @param ext: Extensions to look for, e.g. ".png;.jpg"
std::vector<std::string> getFilenamesInDir(const fs::path& directory, const std::string& ext);

/// Returns the names of all contained directories.
std::vector<std::string> getSubDirectoryNames(const fs::path& dir);

/// Returns to all files in a directory, recursively.
std::vector<fs::path> filePathsRecursive(const fs::path& root);

bool isNewerThan(const fs::path& a, const fs::path& b);

/// relativeTo("foo/bar", "foo/bar/baz") == "baz"
fs::path relativeTo(const fs::path& from, const fs::path& to);

/// Returns the users home dir.
fs::path homeDir();

/// Iff path is ~/foo, it gets expanded to homeDir()/foo, else it is returned as-is.
fs::path expandHome(const fs::path& path);

/// Copy all files recursively from source to destination. Overwrites existing files and folders.
bool copyDir(const fs::path& source, const fs::path& destination);

/// Copy tree structure and create links for the files. Leaves existing files intact.
bool copyDirAsLinks(const fs::path& source, const fs::path& destination);

/// Copy file from source to destination. Overwrites existing files.
bool copyFile(const fs::path& source, const fs::path& destination);

/// Is the file a hidden file?
bool isHidden(const fs::path& path);

size_t fileSizeBytes(const fs::path& path);

time_t lastAccessedTime(const fs::path& path);

/// May the file owner write to the given file?
bool hasWritePermission(const fs::path& path);

/// Set write permission for the file owner.
void setWritePermission(const fs::path& path, bool allow_writing); //NOLINT

bool hasExtension(const fs::path& filepath, const std::string& extension);

bool isObj(const fs::path& filepath);

bool isStl(const fs::path& filepath);

bool isJson(const fs::path& filepath);

bool removeDirectory(const fs::path& dir);
bool createDirectories(const fs::path& dir);

/// Ensure "foo/bar/" exists if pram is "foo/bar/filename.txt"
void createParentPath(const fs::path& file_path);

} // namespace komb
