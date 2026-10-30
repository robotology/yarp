/*
 * SPDX-FileCopyrightText: 2026-2026 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/manager/impl/textparser.h>

#include <yarp/conf/environment.h>

#include <catch2/catch_amalgamated.hpp>
#include <harness.h>

using yarp::manager::ErrorLogger;
using yarp::manager::TextParser;

TEST_CASE("manager::TextParserTest", "[yarp::manager]")
{
    ErrorLogger* logger = ErrorLogger::Instance();
    logger->clear();
    TextParser parser;

    SECTION("Application parameters are kept verbatim without warnings")
    {
        CHECK(parser.parseText("--status $PAR{status=SUCCESS} $PAR{x}") == "--status $PAR{status=SUCCESS} $PAR{x}");
        CHECK(logger->warningCount() == 0);
    }

    SECTION("Variables and environment variables are expanded inside default values")
    {
        yarp::conf::environment::set_string("YARP_MANAGER_TEST_DIR", "/some/dir");
        parser.addVariable("robot", "icub");
        CHECK(parser.parseText("$PAR{dir=$ENV{YARP_MANAGER_TEST_DIR}/conf} $PAR{robot=${robot}}")
              == "$PAR{dir=/some/dir/conf} $PAR{robot=icub}");
        CHECK(logger->warningCount() == 0);
    }

    SECTION("Unknown '$' symbols still produce a warning each, without repeating earlier ones")
    {
        parser.parseText("$foo");
        parser.parseText("$PAR{x} $bar");
        REQUIRE(logger->warningCount() == 2);
        std::string last = logger->getLastWarning();
        const std::string message = "use of symbol '$' detected";
        size_t pos = last.find(message);
        REQUIRE(pos != std::string::npos);
        CHECK(last.find(message, pos + 1) == std::string::npos);
        CHECK(last.find("$PAR{foo=default}") != std::string::npos);
    }

    logger->clear();
}
