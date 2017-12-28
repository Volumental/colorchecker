#pragma once

#include <string>

#include <boost/optional.hpp>

#include <gflags/gflags.h>

#include "Path.hpp"

/*  This is in an own file so that we can initialize logging from
    CLI without needing to include Loguru.
*/

DECLARE_string(logdir);
DECLARE_bool(logfile);
DECLARE_bool(throw_on_fatal);

namespace komb {

enum class Arguments
{
    kNone,       ///< No gflags, no positional
    kOnlyGflags, ///< Just gflags, no positional
    kNoGflags,   ///< No gflags, just positional
};

/// Helper, returns e.g. "20160526_114305.078"
std::string getDateTimeMillisecons();

/// Initalizes gflags and logging.
void initLogging(int& argc, char**& argv, Arguments arguments = Arguments::kOnlyGflags); //NOLINT
void initLoggingNoLogfile(int& argc, char**& argv, Arguments arguments = Arguments::kOnlyGflags); //NOLINT

/// Initialize logging without gflags
void initLoggingNoArgs(const char* app_name);

/// Get useful stats like: program arguments and compilation time.
std::string getProgramStats();

/// Path to the log file containing the gist (same as stderr)
boost::optional<fs::path> getLogPathGist();

/// Path to the log file containing the MAX verbosity
boost::optional<fs::path> getLogPathMax();

} // namespace komb
