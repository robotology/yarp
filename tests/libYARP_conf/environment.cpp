/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/conf/environment.h>

#include <catch.hpp>
#include <harness.h>

#include <array>


namespace env = yarp::conf::environment;
using namespace std::string_literals;

// RAII-style environment variables with prefix + some utilities
class YEnv
{
public:
    YEnv(std::string suffix, std::string value) :
            m_suffix(std::move(suffix)),
            m_value(std::move(value))
    {
        INFO("Setting " + name() + "=\"" + m_value + "\"");

        // Ensure that the variable is not already set
        REQUIRE(!env::is_set(name()));

        // Ensure that the operation was successful
        REQUIRE(env::set_string(name(), m_value));

        // Ensure that the variable is now set
        REQUIRE(env::is_set(name()));

        // Double check if the variable was properly set
        bool ret{false};
        auto val = env::get_string(name(), &ret);
        REQUIRE(ret);
        REQUIRE(val == m_value);
    }

    YEnv(const std::string& value) : YEnv(value, value)
    {
    }

    ~YEnv()
    {
        INFO("Deleting " + name() + " (expected=\"" + m_value + "\")");

        // Ensure that the variable is set
        REQUIRE(env::is_set(name()));

        // Check that the variable was not modified or removed
        bool ret{false};
        auto val = env::get_string(name(), &ret);
        REQUIRE(ret);
        REQUIRE(val == m_value);

        // Check that removing the variable does not fail
        REQUIRE(env::unset(name()));

        // Ensure that the variable is now unset
        REQUIRE(!env::is_set(name()));
    }

    YEnv(const YEnv&) = delete;
    YEnv(YEnv&&) noexcept = delete;
    YEnv& operator=(const YEnv&) = delete;
    YEnv& operator=(YEnv&&) noexcept = delete;

    std::string name() const
    {
        return s_prefix + m_suffix;
    }

    std::string value() const
    {
        return m_value;
    }

    static std::string name(const std::string& env)
    {
        return s_prefix + env;
    }

private:
    static const std::string s_prefix;
    std::string m_suffix;
    std::string m_value;
};

const std::string YEnv::s_prefix{"YARP_CONF_TEST_"};


template <typename T,
          std::enable_if_t<
            std::is_integral<T>::value &&
            !std::is_same<T, bool>::value, bool> = true>

void check_get_numeric()
{
}



TEST_CASE("conf::environment", "[yarp::conf]")
{
    SECTION("Test split_path and join_path")
    {
        std::string p0{"a/b/c"};
        std::string p1{"d/e/f"};
        std::string p2{"g/hij/kl"};

        {
            std::string path;
            INFO("Splitting: \"" + path + "\"");
            auto x = env::split_path(path);
            CHECK(x.empty());

            auto y = env::join_path(x);
            CHECK(y == path);
        }

        {
            // vector of one empty string element are converted to empty string
            std::vector<std::string> x { {} };
            auto y = env::join_path(x);
            CHECK(y.empty());
        }

        {
            std::string path {env::path_separator};
            INFO("Splitting: \"" + path + "\"");
            auto x = env::split_path(path);
            CHECK(x.size() == 2);
            CHECK(x[0].empty());
            CHECK(x[1].empty());

            auto y = env::join_path(x);
            CHECK(y == path);
        }

        {
            std::string path {
                std::string{env::path_separator} +
                env::path_separator};
            INFO("Splitting: \"" + path + "\"");
            auto x = env::split_path(path);
            CHECK(x.size() == 3);
            CHECK(x[0].empty());
            CHECK(x[1].empty());
            CHECK(x[2].empty());

            auto y = env::join_path(x);
            CHECK(y == path);
        }

        {
            const std::string& path {p0};
            INFO("Splitting: \"" + path + "\"");
            auto x = env::split_path(path);
            CHECK(x.size() == 1);
            CHECK(x[0] == p0);

            auto y = env::join_path(x);
            CHECK(y == path);
        }

        {
            std::string path {
                p0 +
                env::path_separator +
                p1};
            INFO("Splitting: \"" + path + "\"");
            auto x = env::split_path(path);
            CHECK(x.size() == 2);
            CHECK(x[0] == p0);
            CHECK(x[1] == p1);

            auto y = env::join_path(x);
            CHECK(y == path);
        }

        {
            std::string path {
                p0 +
                env::path_separator +
                p1 +
                env::path_separator +
                p2};
            INFO("Splitting: \"" + path + "\"");
            auto x = env::split_path(path);
            CHECK(x.size() == 3);
            CHECK(x[0] == p0);
            CHECK(x[1] == p1);
            CHECK(x[2] == p2);

            auto y = env::join_path(x);
            CHECK(y == path);
        }

        {
            std::string path {
                env::path_separator +
                p0 +
                env::path_separator +
                p1 +
                env::path_separator +
                p2};
            INFO("Splitting: \"" + path + "\"");
            auto x = env::split_path(path);
            CHECK(x.size() == 4);
            CHECK(x[0].empty());
            CHECK(x[1] == p0);
            CHECK(x[2] == p1);
            CHECK(x[3] == p2);

            auto y = env::join_path(x);
            CHECK(y == path);
        }

        {
            std::string path {
                p0 +
                env::path_separator +
                env::path_separator +
                p1 +
                env::path_separator +
                p2};
            INFO("Splitting: \"" + path + "\"");
            auto x = env::split_path(path);
            CHECK(x.size() == 4);
            CHECK(x[0] == p0);
            CHECK(x[1].empty());
            CHECK(x[2] == p1);
            CHECK(x[3] == p2);

            auto y = env::join_path(x);
            CHECK(y == path);
        }

        {
            std::string path {
                p0 +
                env::path_separator +
                p1 +
                env::path_separator +
                p2 +
                env::path_separator};
            INFO("Splitting: \"" + path + "\"");
            auto x = env::split_path(path);
            CHECK(x.size() == 4);
            CHECK(x[0] == p0);
            CHECK(x[1] == p1);
            CHECK(x[2] == p2);
            CHECK(x[3].empty());

            auto y = env::join_path(x);
            CHECK(y == path);
        }

        {
            std::string path {
                std::string{env::path_separator} +
                env::path_separator +
                p0 +
                env::path_separator +
                p1 +
                env::path_separator +
                env::path_separator +
                env::path_separator +
                p2 +
                env::path_separator +
                env::path_separator};
            INFO("Splitting: \"" + path + "\"");
            auto x = env::split_path(path);
            CHECK(x.size() == 9);
            CHECK(x[0].empty());
            CHECK(x[1].empty());
            CHECK(x[2] == p0);
            CHECK(x[3] == p1);
            CHECK(x[4].empty());
            CHECK(x[5].empty());
            CHECK(x[6] == p2);
            CHECK(x[7].empty());
            CHECK(x[8].empty());

            auto y = env::join_path(x);
            CHECK(y == path);
        }
    }

    SECTION("Test env::set_string, env::get_string, env::unset() and sanity checks for YEnv class")
    {
        CHECK(env::get_string(YEnv::name("FOO")).empty());
        CHECK(env::get_string(YEnv::name("Foo")).empty());
        CHECK(env::get_string(YEnv::name("foo")).empty());
        {
            const YEnv y1("FOO");
            bool ret1;
            std::cout << y1.name() << " = " << env::get_string(y1.name()) << "\n";
            CHECK(env::get_string(y1.name()) == y1.value());
            auto x1 = env::get_string(y1.name(), &ret1);
            CHECK(x1 == y1.value());
            CHECK(ret1);

#if !defined(_WIN32)
// Environment variable names are case sensitive on windows
            const YEnv y2("Foo");
            bool ret2;
            std::cout << y2.name() << " = " << env::get_string(y2.name()) << "\n";
            CHECK(env::get_string(y2.name()) == y2.value());
            CHECK(env::get_string(y1.name()) == y1.value());
            x1 = env::get_string(y1.name(), &ret1);
            CHECK(x1 == y1.value());
            CHECK(ret1);
            auto x2 = env::get_string(y2.name(), &ret2);
            CHECK(x2 == y2.value());
            CHECK(ret2);

            const YEnv y3("foo");
            bool ret3;
            std::cout << y3.name() << " = " << env::get_string(y3.name()) << "\n";
            CHECK(env::get_string(y3.name()) == y3.value());
            CHECK(env::get_string(y2.name()) == y2.value());
            CHECK(env::get_string(y1.name()) == y1.value());
            x1 = env::get_string(y1.name(), &ret1);
            CHECK(x1 == y1.value());
            CHECK(ret1);
            x2 = env::get_string(y2.name(), &ret2);
            CHECK(x2 == y2.value());
            CHECK(ret2);
            auto x3 = env::get_string(y3.name(), &ret3);
            CHECK(x3 == y3.value());
            CHECK(ret3);
#endif
        }

        CHECK(env::get_string(YEnv::name("FOO")).empty());
        CHECK(env::get_string(YEnv::name("Foo")).empty());
        CHECK(env::get_string(YEnv::name("foo")).empty());

        bool ret1;
        auto x1 = env::get_string(YEnv::name("FOO"), &ret1);
        CHECK(x1.empty());
        CHECK(!ret1);

        bool ret2;
        auto x2 = env::get_string(YEnv::name("Foo"), &ret2);
        CHECK(x2.empty());
        CHECK(!ret2);

        bool ret3;
        auto x3 = env::get_string(YEnv::name("Foo"), &ret3);
        CHECK(x3.empty());
        CHECK(!ret3);
    }

    SECTION("Test env::get_bool")
    {
        const YEnv y1("1");
        const YEnv ytrue("TRUE_LOWERCASE", "true");
        const YEnv yTrue("TRUE_CAMELCASE", "True");
        const YEnv yTRUE("TRUE_UPPERCASE", "TRUE");
        const YEnv yon("ON_LOWERCASE", "on");
        const YEnv yOn("ON_CAMELCASE", "On");
        const YEnv yON("ON_UPPERCASE", "ON");
        const YEnv yyes("YES_LOWERCASE", "yes");
        const YEnv yYes("YES_CAMELCASE", "Yes");
        const YEnv yYES("YES_UPPERCASE", "YES");

        const YEnv y0("0");
        const YEnv yfalse("FALSE_LOWERCASE", "false");
        const YEnv yFalse("FALSE_CAMELCASE", "False");
        const YEnv yFALSE("FALSE_UPPERCASE", "FALSE");
        const YEnv yoff("OFF_LOWERCASE", "off");
        const YEnv yOff("OFF_CAMELCASE", "Off");
        const YEnv yOFF("OFF_UPPERCASE", "OFF");
        const YEnv yno("NO_LOWERCASE", "no");
        const YEnv yNo("NO_CAMELCASE", "No");
        const YEnv yNO("NO_UPPERCASE", "NO");

        const YEnv yOTHER("OTHER");

        CHECK(env::get_bool(y1.name()));
        CHECK(env::get_bool(ytrue.name()));
        CHECK(env::get_bool(yTrue.name()));
        CHECK(env::get_bool(yTRUE.name()));
        CHECK(env::get_bool(yon.name()));
        CHECK(env::get_bool(yOn.name()));
        CHECK(env::get_bool(yON.name()));
        CHECK(env::get_bool(yyes.name()));
        CHECK(env::get_bool(yYes.name()));
        CHECK(env::get_bool(yYES.name()));
        CHECK(!env::get_bool(y0.name()));
        CHECK(!env::get_bool(yfalse.name()));
        CHECK(!env::get_bool(yFalse.name()));
        CHECK(!env::get_bool(yFALSE.name()));
        CHECK(!env::get_bool(yoff.name()));
        CHECK(!env::get_bool(yOff.name()));
        CHECK(!env::get_bool(yOFF.name()));
        CHECK(!env::get_bool(yno.name()));
        CHECK(!env::get_bool(yNo.name()));
        CHECK(!env::get_bool(yNO.name()));

        CHECK(env::get_bool(y1.name(), true));
        CHECK(env::get_bool(ytrue.name(), true));
        CHECK(env::get_bool(yTrue.name(), true));
        CHECK(env::get_bool(yTRUE.name(), true));
        CHECK(env::get_bool(yon.name(), true));
        CHECK(env::get_bool(yOn.name(), true));
        CHECK(env::get_bool(yON.name(), true));
        CHECK(env::get_bool(yyes.name(), true));
        CHECK(env::get_bool(yYes.name(), true));
        CHECK(env::get_bool(yYES.name(), true));
        CHECK(!env::get_bool(y0.name(), true));
        CHECK(!env::get_bool(yfalse.name(), true));
        CHECK(!env::get_bool(yFalse.name(), true));
        CHECK(!env::get_bool(yFALSE.name(), true));
        CHECK(!env::get_bool(yoff.name(), true));
        CHECK(!env::get_bool(yOff.name(), true));
        CHECK(!env::get_bool(yOFF.name(), true));
        CHECK(!env::get_bool(yno.name(), true));
        CHECK(!env::get_bool(yNo.name(), true));
        CHECK(!env::get_bool(yNO.name(), true));

        CHECK(env::get_bool(y1.name(), false));
        CHECK(env::get_bool(ytrue.name(), false));
        CHECK(env::get_bool(yTrue.name(), false));
        CHECK(env::get_bool(yTRUE.name(), false));
        CHECK(env::get_bool(yon.name(), false));
        CHECK(env::get_bool(yOn.name(), false));
        CHECK(env::get_bool(yON.name(), false));
        CHECK(env::get_bool(yyes.name(), false));
        CHECK(env::get_bool(yYes.name(), false));
        CHECK(env::get_bool(yYES.name(), false));
        CHECK(!env::get_bool(y0.name(), false));
        CHECK(!env::get_bool(yfalse.name(), false));
        CHECK(!env::get_bool(yFalse.name(), false));
        CHECK(!env::get_bool(yFALSE.name(), false));
        CHECK(!env::get_bool(yoff.name(), false));
        CHECK(!env::get_bool(yOff.name(), false));
        CHECK(!env::get_bool(yOFF.name(), false));
        CHECK(!env::get_bool(yno.name(), false));
        CHECK(!env::get_bool(yNo.name(), false));
        CHECK(!env::get_bool(yNO.name(), false));

        CHECK(!env::get_bool(yOTHER.name()));
        CHECK(!env::get_bool("THIS_VARIABLE_SHOULD_NOT_BE_SET"));

        CHECK(env::get_bool(yOTHER.name(), true));
        CHECK(env::get_bool("THIS_VARIABLE_SHOULD_NOT_BE_SET", true));

        CHECK(!env::get_bool(yOTHER.name(), false));
        CHECK(!env::get_bool("THIS_VARIABLE_SHOULD_NOT_BE_SET", false));
    }

    SECTION("Test env::set_bool")
    {
        // Preliminary checks
        CHECK(!env::get_bool(YEnv::name("TRUE")));
        CHECK(env::get_bool(YEnv::name("TRUE"), true));
        CHECK(!env::get_bool(YEnv::name("TRUE"), false));
        CHECK(!env::get_bool(YEnv::name("FALSE")));
        CHECK(env::get_bool(YEnv::name("FALSE"), true));
        CHECK(!env::get_bool(YEnv::name("FALSE"), false));

        // Test set_bool
        env::set_bool(YEnv::name("TRUE"), true);
        env::set_bool(YEnv::name("FALSE"), false);
        CHECK(env::get_bool(YEnv::name("TRUE")));
        CHECK(env::get_bool(YEnv::name("TRUE"), true));
        CHECK(env::get_bool(YEnv::name("TRUE"), false));
        CHECK(!env::get_bool(YEnv::name("FALSE")));
        CHECK(!env::get_bool(YEnv::name("FALSE"), false));
        CHECK(!env::get_bool(YEnv::name("FALSE"), false));

        // Clean environment and final sanity checks
        env::unset(YEnv::name("TRUE"));
        env::unset(YEnv::name("FALSE"));
        CHECK(!env::get_bool(YEnv::name("TRUE")));
        CHECK(env::get_bool(YEnv::name("TRUE"), true));
        CHECK(!env::get_bool(YEnv::name("TRUE"), false));
        CHECK(!env::get_bool(YEnv::name("FALSE")));
        CHECK(env::get_bool(YEnv::name("FALSE"), true));
        CHECK(!env::get_bool(YEnv::name("FALSE"), false));
    }

    SECTION("Test env::get_string")
    {
        const std::string foo_default = "foo_default"s;
        const std::string bar_default = "bar_default"s;
        const std::string bar_append = "+bar_append"s;
        const std::string foo = "foo"s;
        const std::string bar = "bar"s;

        // Test basic version without bool ptr (variable unset)
        {
            CHECK(env::get_string(YEnv::name("FOO")).empty());
        }

        // Test basic version without bool ptr (variable set)
        {
            const YEnv yfoo("FOO", foo);
            CHECK(env::get_string(YEnv::name("FOO")) == foo);
        }

        // Test basic version with bool ptr (variable unset)
        {
            bool ret;
            CHECK(env::get_string(YEnv::name("FOO"), &ret).empty());
            CHECK(!ret);
        }

        // Test basic version with bool ptr (variable set)
        {
            const YEnv yfoo("FOO", foo);
            bool ret;
            CHECK(env::get_string(YEnv::name("FOO"), &ret) == foo);
            CHECK(ret);
        }

        // Test version with default value (variable unset)
        {
            CHECK(env::get_string(YEnv::name("FOO"), foo_default) == foo_default);
        }

        // Test version with default value (variable set)
        {
            const YEnv yfoo("FOO", foo);
            CHECK(env::get_string(YEnv::name("FOO"), foo_default) == foo);
        }

        // Test version with fallback and without append (variable unset, fallback unset)
        {
            CHECK(env::get_string(YEnv::name("FOO"), YEnv::name("BAR"), bar_default) == bar_default);
        }

        // Test version with fallback and without append (variable unset, fallback set)
        {
            const YEnv ybar("BAR", bar);
            CHECK(env::get_string(YEnv::name("FOO"), YEnv::name("BAR"), bar_default) == bar);
        }

        // Test version with fallback and without append (variable set, fallback unset)
        {
            const YEnv yfoo("FOO", foo);
            CHECK(env::get_string(YEnv::name("FOO"), YEnv::name("BAR"), bar_default) == foo);
        }

        // Test version with fallback and without append (variable set, fallback set)
        {
            const YEnv yfoo("FOO", foo);
            const YEnv ybar("BAR", bar);
            CHECK(env::get_string(YEnv::name("FOO"), YEnv::name("BAR"), bar_default) == foo);
        }

        // Test version with fallback and with append (variable unset, fallback unset)
        {
            CHECK(env::get_string(YEnv::name("FOO"), YEnv::name("BAR"), bar_default, bar_append) == bar_default + bar_append);
        }

        // Test version with fallback and with append (variable unset, fallback set)
        {
            const YEnv ybar("BAR", bar);
            CHECK(env::get_string(YEnv::name("FOO"), YEnv::name("BAR"), bar_default, bar_append) == bar + bar_append);
        }

        // Test version with fallback and with append (variable set, fallback unset)
        {
            const YEnv yfoo("FOO", foo);
            CHECK(env::get_string(YEnv::name("FOO"), YEnv::name("BAR"), bar_default, bar_append) == foo);
        }

        // Test version with fallback and with append (variable set, fallback set)
        {
            const YEnv yfoo("FOO", foo);
            const YEnv ybar("BAR", bar);
            CHECK(env::get_string(YEnv::name("FOO"), YEnv::name("BAR"), bar_default, bar_append) == foo);
        }
    }


    SECTION("Test env::get_path")
    {
        std::array<std::string, 2> foo_default =
        {
            "/foo/default/0"s,
            "/foo/default/1"s
        };
        auto foo_default_env = yarp::conf::string::join<std::array<std::string, 2>>(foo_default.begin(), foo_default.end(), yarp::conf::environment::path_separator);

        std::array<std::string, 3> bar_default =
        {
            "/bar/default/0"s,
            "/bar/default/1"s,
            "/bar/default/2"s
        };
        auto bar_default_env = yarp::conf::string::join<std::array<std::string, 3>>(bar_default.begin(), bar_default.end(), yarp::conf::environment::path_separator);

        std::array<std::string, 4> foo =
        {
            "/foo/0"s,
            "/foo/1"s,
            "/foo/2"s,
            "/foo/3"s
        };
        auto foo_env = yarp::conf::string::join<std::array<std::string, 4>>(foo.begin(), foo.end(), yarp::conf::environment::path_separator);

        std::array<std::string, 5> bar =
        {
            "/bar/0"s,
            "/bar/1"s,
            "/bar/2"s,
            "/bar/3"s,
            "/bar/4"s
        };
        auto bar_env = yarp::conf::string::join<std::array<std::string, 5>>(bar.begin(), bar.end(), yarp::conf::environment::path_separator);

        const std::string bar_append = env::path_separator + "append"s;


        // Test basic version without bool ptr (variable unset)
        {
            auto x = env::get_path(YEnv::name("FOO"));
            CHECK(x.empty());
        }

        // Test basic version without bool ptr (variable set)
        {
            const YEnv yfoo("FOO", foo_env);
            auto x = env::get_path(YEnv::name("FOO"));
            CHECK(x.size() == foo.size());
            CHECK(std::equal(x.begin(), x.end(), foo.begin(), foo.end()));
        }

        // Test basic version with bool ptr (variable unset)
        {
            bool ret;
            auto x = env::get_path(YEnv::name("FOO"), &ret);
            CHECK(x.empty());
            CHECK(!ret);
        }

        // Test basic version with bool ptr (variable set)
        {
            const YEnv yfoo("FOO", foo_env);
            bool ret;
            auto x = env::get_path(YEnv::name("FOO"), &ret);
            CHECK(x.size() == foo.size());
            CHECK(std::equal(x.begin(), x.end(), foo.begin(), foo.end()));
            CHECK(ret);
        }

        // Test version with default value (variable unset)
        {
            auto x = env::get_path(YEnv::name("FOO"), foo_default_env);
            CHECK(x.size() == foo_default.size());
            CHECK(std::equal(x.begin(), x.end(), foo_default.begin(), foo_default.end()));
        }

        // Test version with default value (variable set)
        {
            const YEnv yfoo("FOO", foo_env);
            auto x = env::get_path(YEnv::name("FOO"), foo_default_env);
            CHECK(x.size() == foo.size());
            CHECK(std::equal(x.begin(), x.end(), foo.begin(), foo.end()));
        }

        // Test version with fallback and without append (variable unset, fallback unset)
        {
            auto x = env::get_path(YEnv::name("FOO"), YEnv::name("BAR"), bar_default_env);
            CHECK(x.size() == bar_default.size()); // FIXME
            CHECK(std::equal(x.begin(), x.end(), bar_default.begin(), bar_default.end())); // FIXME
        }

        // Test version with fallback and without append (variable unset, fallback set)
        {
            const YEnv ybar("BAR", bar_env);
            auto x = env::get_path(YEnv::name("FOO"), YEnv::name("BAR"), bar_default_env);
            CHECK(x.size() == bar.size()); // FIXME
            CHECK(std::equal(x.begin(), x.end(), bar.begin(), bar.end())); // FIXME
        }

        // Test version with fallback and without append (variable set, fallback unset)
        {
            const YEnv yfoo("FOO", foo_env);
            auto x = env::get_path(YEnv::name("FOO"), YEnv::name("BAR"), bar_default_env);
            CHECK(x.size() == foo.size());
            CHECK(std::equal(x.begin(), x.end(), foo.begin(), foo.end())); // FIXME
        }

        // Test version with fallback and without append (variable set, fallback set)
        {
            const YEnv yfoo("FOO", foo_env);
            const YEnv ybar("BAR", bar_env);
            auto x = env::get_path(YEnv::name("FOO"), YEnv::name("BAR"), bar_default_env);
            CHECK(x.size() == foo.size());
            CHECK(std::equal(x.begin(), x.end(), foo.begin(), foo.end())); // FIXME
        }

        // Test version with fallback and with append (variable unset, fallback unset)
        {
            auto x = env::get_path(YEnv::name("FOO"), YEnv::name("BAR"), bar_default_env, bar_append);
            CHECK(x.size() == bar_default.size());
            for (size_t i = 0; i < x.size(); ++i) {
                CHECK(x[i] == bar_default[i] + bar_append);
            }
        }

        // Test version with fallback and with append (variable unset, fallback set)
        {
            const YEnv ybar("BAR", bar_env);
            auto x = env::get_path(YEnv::name("FOO"), YEnv::name("BAR"), bar_default_env, bar_append);
            CHECK(x.size() == bar.size());
            for (size_t i = 0; i < x.size(); ++i) {
                CHECK(x[i] == bar[i] + bar_append);
            }
        }

        // Test version with fallback and with append (variable set, fallback unset)
        {
            const YEnv yfoo("FOO", foo_env);
            auto x = env::get_path(YEnv::name("FOO"), YEnv::name("BAR"), bar_default_env, bar_append);
            CHECK(x.size() == foo.size());
            CHECK(std::equal(x.begin(), x.end(), foo.begin(), foo.end()));
        }

        // Test version with fallback and with append (variable set, fallback set)
        {
            const YEnv yfoo("FOO", foo_env);
            const YEnv ybar("BAR", bar_env);
            auto x = env::get_path(YEnv::name("FOO"), YEnv::name("BAR"), bar_default_env, bar_append);
            CHECK(x.size() == foo.size());
            CHECK(std::equal(x.begin(), x.end(), foo.begin(), foo.end()));
        }
    }
}

TEMPLATE_TEST_CASE("conf::environment::get_numeric", "[yarp::conf]",
    std::uint8_t,
    std::int8_t,
    std::uint16_t,
    std::int16_t,
    std::uint32_t,
    std::int32_t,
    std::uint64_t,
    std::int64_t
)
{
    using T = TestType;
    using uT = std::make_unsigned_t<T>;
    using sT= std::make_signed_t<T>;
    constexpr bool is_unsigned = std::is_unsigned<T>::value;
    constexpr bool is_signed = std::is_signed<T>::value;
    constexpr T default_value = 0;
    constexpr T alt_default_value = 42;

    constexpr auto umin = static_cast<uT>(0);
    constexpr auto umax = std::numeric_limits<uT>::max();
    constexpr auto smin = std::numeric_limits<sT>::min();
    constexpr auto smax = std::numeric_limits<sT>::max();
    constexpr auto smaxpp = static_cast<uT>(smax) + 1;

    const auto umin_s = std::to_string(umin);
    const auto umax_s = std::to_string(umax);
    const auto smin_s = std::to_string(smin);
    const auto smax_s = std::to_string(smax);
    const auto smaxpp_s = std::to_string(smaxpp);

    const YEnv yumin("UMIN", umin_s);
    const YEnv yumax("UMAX", umax_s);
    const YEnv ysmin("SMIN", smin_s);
    const YEnv ysmax("SMAX", smax_s);
    const YEnv ysmaxpp("SMAXPP", smaxpp_s);

    const YEnv y1("1");
    const YEnv y2("2");
    const YEnv yneg1("neg1", "-1");
    const YEnv yneg2("neg2", "-2");

    const YEnv ynot_numeric("NOT_NUMERIC");
    const YEnv y20error("20ERROR");

    SECTION("Test default value")
    {
        CHECK(env::get_numeric<T>(yumin.name()) == umin);
        CHECK(env::get_numeric<T>(yumax.name()) == (is_unsigned ? umax : default_value));
        CHECK(env::get_numeric<T>(ysmin.name()) == (is_signed ? smin : default_value));
        CHECK(env::get_numeric<T>(ysmax.name()) == smax);
        CHECK(env::get_numeric<T>(ysmaxpp.name()) == (is_unsigned ? smaxpp : default_value));
        CHECK(env::get_numeric<T>(y1.name()) == 1);
        CHECK(env::get_numeric<T>(y2.name()) == 2);
        CHECK(env::get_numeric<T>(yneg1.name()) == (is_signed ? -1 : default_value));
        CHECK(env::get_numeric<T>(yneg2.name()) == (is_signed ? -2 : default_value));
        CHECK(env::get_numeric<T>(ynot_numeric.name()) == default_value);
        CHECK(env::get_numeric<T>(y20error.name()) == default_value);
    }

    SECTION("Test alternative default value")
    {
        CHECK(env::get_numeric<T>(yumin.name(), alt_default_value) == umin);
        CHECK(env::get_numeric<T>(yumax.name(), alt_default_value) == (is_unsigned ? umax : alt_default_value));
        CHECK(env::get_numeric<T>(ysmin.name(), alt_default_value) == (is_signed ? smin : alt_default_value));
        CHECK(env::get_numeric<T>(ysmax.name(), alt_default_value) == smax);
        CHECK(env::get_numeric<T>(ysmaxpp.name(), alt_default_value) == (is_unsigned ? smaxpp : alt_default_value));
        CHECK(env::get_numeric<T>(y1.name(), alt_default_value) == 1);
        CHECK(env::get_numeric<T>(y2.name(), alt_default_value) == 2);
        CHECK(env::get_numeric<T>(yneg1.name(), alt_default_value) == (is_signed ? -1 : alt_default_value));
        CHECK(env::get_numeric<T>(yneg2.name(), alt_default_value) == (is_signed ? -2 : alt_default_value));
        CHECK(env::get_numeric<T>(ynot_numeric.name(), alt_default_value) == alt_default_value);
        CHECK(env::get_numeric<T>(y20error.name(), alt_default_value) == alt_default_value);
    }
}
