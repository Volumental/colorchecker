#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_NO_MAIN

#include <string>

#include <common/Logging.hpp>
#include <common/LoggingInit.hpp>

#include "teamcity/teamcity_boost.cpp"
#include "teamcity/teamcity_messages.cpp"

DEFINE_string(run_test, "",
    "Run specific test, e.g. \"komb/Vault\" or \"komb/Vault/Vault_Cloud_Client_Rights\"");

int main(int argc, char* argv[])
{
    // Don't write spam generated during testing - only what BOOST outputs:
    loguru::g_stderr_verbosity = loguru::Verbosity_OFF;

    // Allow for --logdir in verify_commit.sh and -v when running manually:
    komb::initLogging(argc, argv, komb::Arguments::kOnlyGflags);

    if (FLAGS_run_test != "")
    {
        int fake_argc = 2;
        std::string argument = "--run_test=" + FLAGS_run_test;
        char* fake_argv[3] = {
            argv[0],
            const_cast<char*>(argument.c_str()),
            nullptr,
        };
        return ::boost::unit_test::unit_test_main([](){return true;}, fake_argc, fake_argv);
    }
    else
    {
        return ::boost::unit_test::unit_test_main([](){return true;}, argc, argv);
    }
}
