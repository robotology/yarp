/*
 * Copyright (C) 2006-2020 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#define CATCH_CONFIG_RUNNER
#include <catch.hpp>

#include <yarp/conf/filesystem.h>
#include <yarp/os/Network.h>
#include <yarp/os/Property.h>
#include <yarp/os/NameStore.h>
#include <yarp/os/YarpPlugin.h>

#include <yarp/serversql/yarpserversql.h>

#include <YarpBuildLocation.h>

int yarp_tests_skipped = 0;

namespace {

static yarp::os::Network* net = nullptr;
static yarp::os::NameStore* store = nullptr;
static bool verbose = false;
static bool no_bypass = false;

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
            std::string{yarp::conf::filesystem::path_separator}  +
            TEST_DATA_DIR;
    yarp::os::NetworkBase::setEnvironment("YARP_DATA_DIRS", yarp_data_dirs);

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
    yarp::os::NetworkBase::setEnvironment("YARP_DATA_HOME", yarp_data_home);

    // To ensure that this will behave in the same way if YARP is configured on
    // the user's system and on the build machines, YARP_CONFIG_DIRS and
    // YARP_CONFIG_HOME are set to a non existent directory
    std::string yarp_config_dirs = CMAKE_BINARY_DIR +
            std::string{yarp::conf::filesystem::preferred_separator} +
            "etc" +
            std::string{yarp::conf::filesystem::preferred_separator} +
            "yarp";
    yarp::os::NetworkBase::setEnvironment("YARP_CONFIG_DIRS", yarp_config_dirs);

    std::string yarp_config_home = CMAKE_BINARY_DIR +
            std::string{yarp::conf::filesystem::preferred_separator} +
            "home" +
            std::string{yarp::conf::filesystem::preferred_separator} +
            "user" +
            std::string{yarp::conf::filesystem::preferred_separator} +
            ".config" +
            std::string{yarp::conf::filesystem::preferred_separator} +
            "yarp";
    yarp::os::NetworkBase::setEnvironment("YARP_CONFIG_HOME", yarp_config_home);

    if (verbose) {
        printf("YARP_DATA_DIRS=\"%s\"\n", yarp::os::NetworkBase::getEnvironment("YARP_DATA_DIRS").c_str());
        printf("YARP_DATA_HOME=\"%s\"\n", yarp::os::NetworkBase::getEnvironment("YARP_DATA_HOME").c_str());
        printf("YARP_CONFIG_DIRS=\"%s\"\n", yarp::os::NetworkBase::getEnvironment("YARP_CONFIG_DIRS").c_str());
        printf("YARP_CONFIG_HOME=\"%s\"\n", yarp::os::NetworkBase::getEnvironment("YARP_CONFIG_HOME").c_str());
    }
}

static void init_Network()
{
    net = new yarp::os::Network;
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

} // namespace


int main(int argc, char *argv[])
{
    Catch::Session session;

    auto cli = session.cli() | Catch::clara::Opt(verbose)["--yarp-verbose"]("Enable verbose mode")
                             | Catch::clara::Opt(no_bypass)["--yarp-no-bypass"]("Do not bypass yarpserver");
    session.cli( cli );

    int returnCode = session.applyCommandLine( argc, argv );
    if( returnCode != 0 ) {
        return returnCode;
    }

    setup_Environment();

    init_Network();
    init_NameStore();

    int assertions_failed = session.run();

    fini_NameStore();
    fini_Network();

    // 0 = All assertion passed
    // 255 = All assertion passed, one or more tests skipped
    // 1-254 = The number of assertion failed (max 254)
    // Every test skipped generates also a failed assertion, therefore
    // the number of skips is subtracted.
    assertions_failed = std::min(assertions_failed - yarp_tests_skipped, 254);
    return (assertions_failed ? assertions_failed : (yarp_tests_skipped ? 255 : 0));
}
