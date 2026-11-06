/*
 * SPDX-FileCopyrightText: 2026-2026 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/manager/impl/paramresolver.h>

#include <catch2/catch_amalgamated.hpp>
#include <harness.h>

using yarp::manager::ParamResolver;

namespace {

// Collects, merges with defaults and overrides and resolves text as a whole application would
bool resolveAll(const std::string& text,
                const ParamResolver::ParamMap& defaults,
                const ParamResolver::ParamMap& overrides,
                std::string& out,
                std::string& error)
{
    ParamResolver::NameSet used;
    ParamResolver::ParamMap values;
    return ParamResolver::collect(text, used, error)
        && ParamResolver::merge(used, defaults, overrides, values, error)
        && ParamResolver::resolve(text, values, out, error);
}

} // namespace

TEST_CASE("manager::ParamResolverTest", "[yarp::manager]")
{
    std::string out;
    std::string error;
    ParamResolver::NameSet used;
    ParamResolver::ParamMap values;

    SECTION("Text without placeholders is left untouched")
    {
        for (const std::string text : {"", "/robot/port:o", "${var} $ENV{HOME} $ alone", "PAR{x} $PAR x}"}) {
            used.clear();
            CHECK(ParamResolver::collect(text, used, error));
            CHECK(used.empty());
            CHECK(ParamResolver::resolve(text, {}, out, error));
            CHECK(out == text);
        }
    }

    SECTION("Default values are used when there is no override")
    {
        CHECK(resolveAll("--status $PAR{status}", {{"status", "SUCCESS"}}, {}, out, error));
        CHECK(out == "--status SUCCESS");
    }

    SECTION("Overrides win over default values")
    {
        CHECK(resolveAll("--status $PAR{status}", {{"status", "SUCCESS"}}, {{"status", "RUNNING"}}, out, error));
        CHECK(out == "--status RUNNING");
    }

    SECTION("Several, repeated and adjacent placeholders")
    {
        CHECK(resolveAll("/$PAR{robot}/$PAR{part}$PAR{suffix} $PAR{robot}",
                         {{"robot", "icub"}, {"part", "head"}, {"suffix", ":o"}},
                         {{"part", "left_arm"}}, out, error));
        CHECK(out == "/icub/left_arm:o icub");
    }

    SECTION("Empty default values are valid")
    {
        CHECK(resolveAll("a$PAR{x}b", {{"x", ""}}, {}, out, error));
        CHECK(out == "ab");
    }

    SECTION("Default values written inline are an error that suggests def-param-value")
    {
        CHECK_FALSE(ParamResolver::collect("--status $PAR{status=SUCCESS}", used, error));
        CHECK(error.find("<def-param-value name=\"status\">SUCCESS</def-param-value>") != std::string::npos);
    }

    SECTION("A parameter without default and without override is an error")
    {
        CHECK_FALSE(resolveAll("--robot $PAR{robot}", {}, {}, out, error));
        CHECK(error.find("'robot'") != std::string::npos);
        CHECK(resolveAll("--robot $PAR{robot}", {}, {{"robot", "icub"}}, out, error));
        CHECK(out == "--robot icub");
    }

    SECTION("Overriding a parameter that is neither used nor has a default is an error")
    {
        CHECK(ParamResolver::collect("$PAR{x}", used, error));
        CHECK_FALSE(ParamResolver::merge(used, {{"x", "1"}}, {{"y", "2"}}, values, error));
        CHECK(error.find("'y'") != std::string::npos);
    }

    SECTION("A parameter with a default can be overridden even if it is not used")
    {
        CHECK(ParamResolver::merge(used, {{"x", "1"}}, {{"x", "2"}}, values, error));
        CHECK(values == ParamResolver::ParamMap {{"x", "2"}});
    }

    SECTION("Malformed placeholders are errors")
    {
        for (const std::string text : {"$PAR{x", "a $PAR{x} b $PAR{y", "$PAR{}", "$PAR{=1}", "$PAR{a=1}", "$PAR{a$PAR{b}}"}) {
            used.clear();
            CHECK_FALSE(ParamResolver::collect(text, used, error));
            CHECK_FALSE(error.empty());
            CHECK_FALSE(ParamResolver::resolve(text, {{"x", "1"}, {"a", "1"}}, out, error));
        }
    }

    SECTION("Values are not parsed again")
    {
        CHECK(resolveAll("$PAR{x}", {}, {{"x", "$PAR{y}"}}, out, error));
        CHECK(out == "$PAR{y}");
    }

    SECTION("Other placeholders are left to the TextParser")
    {
        CHECK(resolveAll("${var} $ENV{HOME} $PAR{x}", {{"x", "1"}}, {}, out, error));
        CHECK(out == "${var} $ENV{HOME} 1");
    }
}
