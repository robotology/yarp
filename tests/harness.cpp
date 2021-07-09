/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#define CATCH_CONFIG_RUNNER
#include <catch.hpp>

#include <yarp/conf/environment.h>
#include <yarp/conf/filesystem.h>

#if !defined(WITHOUT_NETWORK)
#  include <yarp/os/Network.h>
#  include <yarp/os/Property.h>
#  include <yarp/os/NameStore.h>
#  include <yarp/os/YarpPlugin.h>

#  include <yarp/serversql/yarpserversql.h>
#endif // WITHOUT_NETWORK

#include <YarpBuildLocation.h>

int yarp_tests_skipped = 0;

namespace {

static bool verbose = false;

static void setup_Environment()
{
    // To make sure that the dev test are able to find all and only the devices
    // compiled by YARP, including the ones compiled as dynamic plugins,
    // YARP_DATA_DIRS is set to the build directory + the TEST_DATA_DIR
    // and YARP_DATA_HOME is set to a non existent directory
    std::string yarp_data_dirs =
            CMAKE_BINARY_DIR +
            std::string{yarp::conf::filesystem::preferred_separator} +
            "share" +
            std::string{yarp::conf::filesystem::preferred_separator} +
            "yarp" +
            std::string{yarp::conf::environment::path_separator}  +
            TEST_DATA_DIR;
    yarp::conf::environment::set_string("YARP_DATA_DIRS", yarp_data_dirs);

    std::string yarp_data_home =
            CMAKE_BINARY_DIR +
            std::string{yarp::conf::filesystem::preferred_separator} +
            "home" +
            std::string{yarp::conf::filesystem::preferred_separator} +
            "user" +
            std::string{yarp::conf::filesystem::preferred_separator} +
            ".local" +
            std::string{yarp::conf::filesystem::preferred_separator} +
            "yarp";
    yarp::conf::environment::set_string("YARP_DATA_HOME", yarp_data_home);

    // To ensure that this will behave in the same way if YARP is configured on
    // the user's system and on the build machines, YARP_CONFIG_DIRS and
    // YARP_CONFIG_HOME are set to a non existent directory
    std::string yarp_config_dirs = CMAKE_BINARY_DIR +
            std::string{yarp::conf::filesystem::preferred_separator} +
            "etc" +
            std::string{yarp::conf::filesystem::preferred_separator} +
            "yarp";
    yarp::conf::environment::set_string("YARP_CONFIG_DIRS", yarp_config_dirs);

    std::string yarp_config_home = CMAKE_BINARY_DIR +
            std::string{yarp::conf::filesystem::preferred_separator} +
            "home" +
            std::string{yarp::conf::filesystem::preferred_separator} +
            "user" +
            std::string{yarp::conf::filesystem::preferred_separator} +
            ".config" +
            std::string{yarp::conf::filesystem::preferred_separator} +
            "yarp";
    yarp::conf::environment::set_string("YARP_CONFIG_HOME", yarp_config_home);

    if (verbose) {
        printf("YARP_DATA_DIRS=\"%s\"\n", yarp::conf::environment::get_string("YARP_DATA_DIRS").c_str());
        printf("YARP_DATA_HOME=\"%s\"\n", yarp::conf::environment::get_string("YARP_DATA_HOME").c_str());
        printf("YARP_CONFIG_DIRS=\"%s\"\n", yarp::conf::environment::get_string("YARP_CONFIG_DIRS").c_str());
        printf("YARP_CONFIG_HOME=\"%s\"\n", yarp::conf::environment::get_string("YARP_CONFIG_HOME").c_str());
    }
}

#if !defined(WITHOUT_NETWORK)

static bool no_bypass = false;

static yarp::os::Network* net = nullptr;
static yarp::os::NameStore* store = nullptr;

static void init_Network()
{
    net = new yarp::os::Network;
#ifndef YARP_NO_DEPRECATED // Since YARP 3.4
YARP_WARNING_PUSH
YARP_DISABLE_DEPRECATED_WARNING
    if (verbose) {
        yarp::os::NetworkBase::setVerbosity(1);
    }
YARP_WARNING_POP
#endif // YARP_NO_DEPRECATED
}

static void fini_Network()
{
    delete net;
    net = nullptr;
}

static void init_NameStore()
{
    assert(net != nullptr);

    if (!no_bypass) {
        yarp::os::Property opts;
        opts.put("portdb",":memory:");
        opts.put("subdb",":memory:");
        opts.put("local",1);
        if (verbose) {
            opts.put("verbose", 1);
        }
        store = yarpserver_create(opts);
        net->queryBypass(store);
    } else {
        printf("WARNING: Not bypassing yarp server\n");
    }

}

static void fini_NameStore()
{
    if (!no_bypass) {
        net->queryBypass(nullptr);
        delete store;
        store = nullptr;
    }
}

#endif // WITHOUT_NETWORK

} // namespace


int main(int argc, char *argv[])
{
    Catch::Session session;

    auto cli = session.cli() | Catch::clara::Opt(verbose)["--yarp-verbose"]("Enable verbose mode")
#if !defined(WITHOUT_NETWORK)
                             | Catch::clara::Opt(no_bypass)["--yarp-no-bypass"]("Do not bypass yarpserver");
#else
                             ;
#endif // WITHOUT_NETWORK

    session.cli( cli );

    int returnCode = session.applyCommandLine( argc, argv );
    if( returnCode != 0 ) {
        return returnCode;
    }

    setup_Environment();

#if !defined(WITHOUT_NETWORK)
    init_Network();
    init_NameStore();
#endif // WITHOUT_NETWORK

    int assertions_failed = session.run();

#if !defined(WITHOUT_NETWORK)
    fini_NameStore();
    fini_Network();
#endif // WITHOUT_NETWORK

    // 0 = All assertion passed
    // 255 = Probably an assert was hit and the program exited somewhere else
    // 254 = All assertion passed, one or more tests skipped
    // 1-253 = The number of assertion failed (max 253)
    // Every test skipped generates also a failed assertion, therefore
    // the number of skips is subtracted.
    assertions_failed = std::min(assertions_failed - yarp_tests_skipped, 253);
    return (assertions_failed ? assertions_failed : (yarp_tests_skipped ? 254 : 0));
}
