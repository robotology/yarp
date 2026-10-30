/*
 * SPDX-FileCopyrightText: 2026-2026 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/manager/impl/paramresolver.h>

#include <catch2/catch_amalgamated.hpp>
#include <harness.h>

using yarp::manager::ParamResolver;

namespace {

// Collects, merges with overrides and resolves text as a whole application would
bool resolveAll(const std::string& text,
                const ParamResolver::ParamMap& overrides,
                std::string& out,
                std::string& error)
{
    ParamResolver::DeclMap decls;
    ParamResolver::ParamMap values;
    return ParamResolver::collect(text, decls, error)
        && ParamResolver::merge(decls, overrides, values, error)
        && ParamResolver::resolve(text, values, out, error);
}

} // namespace

TEST_CASE("manager::ParamResolverTest", "[yarp::manager]")
{
    std::string out;
    std::string error;
    ParamResolver::DeclMap decls;
    ParamResolver::ParamMap values;

    SECTION("Text without placeholders is left untouched")
    {
        for (const std::string text : {"", "/robot/port:o", "${var} $ENV{HOME} $ alone", "PAR{x} $PAR x}"}) {
            decls.clear();
            CHECK(ParamResolver::collect(text, decls, error));
            CHECK(decls.empty());
            CHECK(ParamResolver::resolve(text, {}, out, error));
            CHECK(out == text);
        }
    }

    SECTION("Default values are used when there is no override")
    {
        CHECK(resolveAll("--status $PAR{status=SUCCESS}", {}, out, error));
        CHECK(out == "--status SUCCESS");
    }

    SECTION("Overrides win over default values")
    {
        CHECK(resolveAll("--status $PAR{status=SUCCESS}", {{"status", "RUNNING"}}, out, error));
        CHECK(out == "--status RUNNING");
    }

    SECTION("Several, repeated and adjacent placeholders")
    {
        CHECK(resolveAll("/$PAR{robot=icub}/$PAR{part=head}$PAR{suffix=:o} $PAR{robot}",
                         {{"part", "left_arm"}}, out, error));
        CHECK(out == "/icub/left_arm:o icub");
    }

    SECTION("Default value edge cases")
    {
        CHECK(ParamResolver::collect("$PAR{a=} $PAR{b=x=y} $PAR{c}", decls, error));
        REQUIRE(decls.size() == 3);
        CHECK(decls["a"].hasDefault);
        CHECK(decls["a"].defaultValue.empty());
        CHECK(decls["b"].defaultValue == "x=y");
        CHECK_FALSE(decls["c"].hasDefault);
    }

    SECTION("Same default twice, or with a bare placeholder, is accepted in any order")
    {
        CHECK(resolveAll("$PAR{x} $PAR{x=1} $PAR{x=1} $PAR{x}", {}, out, error));
        CHECK(out == "1 1 1 1");
    }

    SECTION("Two different defaults for the same parameter are an error")
    {
        CHECK_FALSE(ParamResolver::collect("$PAR{x=1} $PAR{x=2}", decls, error));
        CHECK(error.find("'x'") != std::string::npos);
    }

    SECTION("Conflicting defaults are detected across texts of the same application")
    {
        CHECK(ParamResolver::collect("--a $PAR{x=1}", decls, error));
        CHECK_FALSE(ParamResolver::collect("--b $PAR{x=2}", decls, error));
    }

    SECTION("A parameter without default and without override is an error")
    {
        CHECK_FALSE(resolveAll("--robot $PAR{robot}", {}, out, error));
        CHECK(error.find("'robot'") != std::string::npos);
        CHECK(resolveAll("--robot $PAR{robot}", {{"robot", "icub"}}, out, error));
        CHECK(out == "--robot icub");
    }

    SECTION("Overriding a parameter that is never used is an error")
    {
        CHECK(ParamResolver::collect("$PAR{x=1}", decls, error));
        CHECK_FALSE(ParamResolver::merge(decls, {{"y", "2"}}, values, error));
        CHECK(error.find("'y'") != std::string::npos);
    }

    SECTION("Malformed placeholders are errors")
    {
        for (const std::string text : {"$PAR{x", "a $PAR{x=1} b $PAR{y", "$PAR{}", "$PAR{=1}", "$PAR{a=$PAR{b}}"}) {
            decls.clear();
            CHECK_FALSE(ParamResolver::collect(text, decls, error));
            CHECK_FALSE(error.empty());
            CHECK_FALSE(ParamResolver::resolve(text, {{"x", "1"}, {"a", "1"}}, out, error));
        }
    }

    SECTION("Values are not parsed again")
    {
        CHECK(resolveAll("$PAR{x}", {{"x", "$PAR{y}"}}, out, error));
        CHECK(out == "$PAR{y}");
    }

    SECTION("Other placeholders are left to the TextParser")
    {
        CHECK(resolveAll("${var} $ENV{HOME} $PAR{x=1}", {}, out, error));
        CHECK(out == "${var} $ENV{HOME} 1");
    }
}
