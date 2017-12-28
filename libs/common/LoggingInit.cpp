#include "LoggingInit.hpp"

#include <algorithm>
#include <atomic>
#include <chrono>
#include <cstdarg>
#include <string>
#include <thread>
#include <unistd.h>
#include <vector>

#include <boost/algorithm/string.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/filesystem.hpp>
#include <opencv2/core/core.hpp>

#include "algorithm/Range.hpp"
#include "algorithm/Vector.hpp"
#include "BuildInfo.hpp"
#include "Logging.hpp"
#include "Profiler.hpp"
#include "ScopeExit.hpp"
#include "String.hpp"

// From: https://github.com/BVLC/caffe/pull/891/files
// gflags 2.1 issue: namespace google was changed to gflags without warning.
// The absence of GFLAGS_GFLAGS_H_ indicates if gflags 2.1 is used.
#ifndef GFLAGS_GFLAGS_H_
    namespace gflags = google;
#endif // GFLAGS_GFLAGS_H_

// This defines the symbols needed by loguru:
#define LOGURU_IMPLEMENTATION 1
#include <loguru.hpp>

DEFINE_bool(logfile, true, "Turn logging to file on/off.");
DEFINE_string(logdir, "~/volumental/logs/", "Specify where log files end up.");
DEFINE_bool(throw_on_fatal , false, "Throw an exception of CHECK failures etc?");

DEFINE_bool(profile, false, "Profile this app using Remotery?");

namespace fs = boost::filesystem;

namespace komb {

const bool kRemoveFlags = true;

std::string s_program_stats;

boost::optional<fs::path> s_log_path_gist;
boost::optional<fs::path> s_log_path_max;

// ----------------------------------------------------------------------------

std::string getDateTimeMillisecons()
{
    char date_time[128];
    loguru::write_date_time(date_time, sizeof(date_time));
    return date_time;
}

// TODO(Emil): it would be cleaner to use komb::formatLocalTime in timing/Date.cpp,
// but the library `timing` already depends on this (common).
static std::string formatLocalTime(time_t time_utc)
{
    std::tm timeinfo_local;
    CHECK_NOTNULL(::localtime_r(&time_utc, &timeinfo_local));
    char buffer[256];
    strftime(buffer, sizeof(buffer) - 1, "%Y-%m-%d %H:%M:%S", &timeinfo_local);
    return buffer;
}

std::string getProgramStats()
{
    return s_program_stats;
}

static void startLoguruFileLogging()
{
    if (!FLAGS_logfile)
    {
        LOG(WARNING) << "Logging to file disabled with -nologfile";
        return;
    }

    const auto app_log_root = fs::path(FLAGS_logdir) / loguru::argv0_filename();
    const auto date_time = getDateTimeMillisecons();

    const auto latest_gist_path = app_log_root / strprintf("info.%s.log", date_time.c_str());
    const auto gist_verbosity =
        std::max<loguru::Verbosity>(loguru::Verbosity_INFO, loguru::g_stderr_verbosity);
    loguru::add_file(latest_gist_path.c_str(), loguru::FileMode::Truncate, gist_verbosity);

    const auto latest_max_path = app_log_root / "max.latest.log";
    loguru::add_file(latest_max_path.c_str(), loguru::FileMode::Truncate, loguru::Verbosity_MAX);

    // TODO(Emil): add machine name and username to log file names, like GLOG?
    // Glog log name: name_of_binary.hostname.username.log.INFO.20151002-172129.18906

    s_log_path_gist = latest_gist_path;
    s_log_path_max = latest_max_path;
}

std::string quoteAndJoin(const std::vector<std::string>& strings)
{
    const auto quote = [](const std::string& str) { return '\'' + str + '\''; };
    return boost::algorithm::join(map(strings, quote), " ");
}

static const char* orNull(const char* str)
{
    return str ? str : "NULL";
}

std::string generateProgramStats(char** argv) // NOLINT
{
    std::string binary_date;

    try
    {
        binary_date = formatLocalTime(fs::last_write_time(argv[0]));
    }
    catch (...)
    {
        binary_date = "Failed to figure out time of binary.\n";
    }

    std::string runtime_info;
    runtime_info += strprintf("Binary date:          %s\n", binary_date.c_str());
    runtime_info += strprintf("Current dir:          %s\n", loguru::current_dir());
    runtime_info += strprintf("Arguments:            %s\n", loguru::arguments());
    runtime_info += strprintf("hardware_concurrency: %u\n", std::thread::hardware_concurrency());

#ifdef _WIN32
    runtime_info += strprintf("$USERPROFILE:         %s\n", orNull(getenv("USERPROFILE")));
#else // _WIN32
    runtime_info += strprintf("$HOME:                %s\n", orNull(getenv("HOME")));
#endif // _WIN32

    runtime_info += strprintf("$USER:                %s\n", orNull(getenv("USER")));

    char hostname[256];
    if (gethostname(hostname, 256) != 0)
    {
        strcpy(hostname, "<unknown>"); // NOLINT
    }
    runtime_info += strprintf("hostname:             %s\n", hostname);

    boost::algorithm::trim(runtime_info);

    std::string build_info = kBuildInfo;
    boost::algorithm::trim(build_info);

    return "Build info:\n" + indent(build_info) + "\nRuntime info:\n" + indent(runtime_info);
}

void initLogging(int& argc, char**& argv, Arguments arguments) // NOLINT
{
    CHECK_GE(argc, 1);
    loguru::init(argc, argv); // May remove "-v VERBOSITY" from argc/argv.

    if (arguments == Arguments::kOnlyGflags)
    {
        gflags::ParseCommandLineFlags(&argc, &argv, kRemoveFlags);
    }

    loguru::set_fatal_handler([](const loguru::Message& message)
    {
        if (FLAGS_throw_on_fatal)
        {
            // throw, not LOG_THROW: Loguru has already logged stack trace etc.
            throw std::runtime_error(message.message); //NOLINT
        }
    });

    startLoguruFileLogging();

    if (arguments != Arguments::kNoGflags)
    {
        CHECK_EQ(argc, 1) << "Expected no positional arguments, got: " <<
            quoteAndJoin(std::vector<std::string>(argv + 1, argv + argc));
    }

    cv::setBreakOnError(true); // SIGSEGV instead of exceptions so we get stack traces.

    s_program_stats = generateProgramStats(argv);
    VLOG(1) << "Stats:\n" << getProgramStats();

    if (FLAGS_profile)
    {
        startProfilerServer();
    }
}

void initLoggingNoArgs(const char* app_name)
{
    std::vector<char> app_name_cpy(app_name, app_name + strlen(app_name) + 1);
    int argc = 1;
    char* argv_array[2] = { app_name_cpy.data(), 0 };
    char** argv = argv_array;
    initLogging(argc, argv, Arguments::kNone);
}

void initLoggingNoLogfile(int& argc, char**& argv, Arguments arguments) // NOLINT
{
    FLAGS_logfile = false;
    initLogging(argc, argv, arguments);
}

boost::optional<fs::path> getLogPathGist()
{
    return s_log_path_gist;
}

boost::optional<fs::path> getLogPathMax()
{
    return s_log_path_max;
}

} // namespace komb
