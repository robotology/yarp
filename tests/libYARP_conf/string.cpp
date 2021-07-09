/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/conf/string.h>

#include <catch.hpp>
#include <harness.h>

namespace str = yarp::conf::string;
using namespace std::string_literals;

TEST_CASE("conf::string", "[yarp::conf]")
{
    SECTION("Test join with default separator")
    {
        const std::string p0{"abc"s};
        const std::string p1{"def"s};
        const std::string p2{"ghijkl"s};
        const std::string sep{", "s}; // default separator

        {
            std::vector<std::string> x;
            auto y = str::join(x.begin(), x.end());
            INFO("Joined string: \"" + y + "\"");
            CHECK(y.empty());
        }

        {
            // vector of one empty string element are converted to empty string
            std::vector<std::string> x { {} };
            auto y = str::join(x.begin(), x.end());
            INFO("Joined string: \"" + y + "\"");
            CHECK(y.empty());
        }

        {
            std::vector<std::string> x {{}, {}};
            auto y = str::join(x.begin(), x.end());
            INFO("Joined string: \"" + y + "\"");
            CHECK(y == sep);
        }

        {
            std::vector<std::string> x {{}, {}, {}};
            auto y = str::join(x.begin(), x.end());
            INFO("Joined string: \"" + y + "\"");
            CHECK(y == (sep + sep));
        }

        {
            std::vector<std::string> x {p0};
            auto y = str::join(x.begin(), x.end());
            INFO("Joined string: \"" + y + "\"");
            CHECK(y == p0);
        }

        {
            std::vector<std::string> x {p0, p1};
            auto y = str::join(x.begin(), x.end());
            INFO("Joined string: \"" + y + "\"");
            CHECK(y == (p0 + sep + p1));
        }

        {
            std::vector<std::string> x {p0, p1, p2};
            auto y = str::join(x.begin(), x.end());
            INFO("Joined string: \"" + y + "\"");
            CHECK(y == (p0 + sep + p1 + sep + p2));
        }

        {
            std::vector<std::string> x {{}, p0, p1, p2};
            auto y = str::join(x.begin(), x.end());
            INFO("Joined string: \"" + y + "\"");
            CHECK(y == (sep + p0 + sep + p1 + sep + p2));
        }

        {
            std::vector<std::string> x {p0, {}, p1, p2};
            auto y = str::join(x.begin(), x.end());
            INFO("Joined string: \"" + y + "\"");
            CHECK(y == (p0 + sep + sep + p1 + sep + p2));
        }

        {
            std::vector<std::string> x {p0, p1, p2, {}};
            auto y = str::join(x.begin(), x.end());
            INFO("Joined string: \"" + y + "\"");
            CHECK(y == (p0 + sep + p1 + sep + p2 + sep));
        }

        {
            std::vector<std::string> x {{}, {}, p0, p1, {}, {}, p2, {}, {}};
            auto y = str::join(x.begin(), x.end());
            INFO("Joined string: \"" + y + "\"");
            CHECK(y == (sep + sep + p0 + sep + p1 + sep + sep + sep + p2 + sep + sep));
        }
    }


    SECTION("Test join with custom separator")
    {
        const std::string p0{"abc"s};
        const std::string p1{"def"s};
        const std::string p2{"ghijkl"s};
        const std::string sep{""s};
        std::vector<std::string> vsep
        {
            "---"s,
            "-+"s,
            "^"s,
            "("s,
            "<!>"s
            ""s
        };

        for (const auto& sep : vsep) {

            {
                std::vector<std::string> x;
                auto y = str::join(x.begin(), x.end(), sep);
                INFO("Joined string: \"" + y + "\"");
                CHECK(y.empty());
            }

            {
                // vector of one empty string element are converted to empty string
                std::vector<std::string> x { {} };
                auto y = str::join(x.begin(), x.end(), sep);
                INFO("Joined string: \"" + y + "\"");
                CHECK(y.empty());
            }

            {
                std::vector<std::string> x {{}, {}};
                auto y = str::join(x.begin(), x.end(), sep);
                INFO("Joined string: \"" + y + "\"");
                CHECK(y == sep);
            }

            {
                std::vector<std::string> x {{}, {}, {}};
                auto y = str::join(x.begin(), x.end(), sep);
                INFO("Joined string: \"" + y + "\"");
                CHECK(y == (sep + sep));
            }

            {
                std::vector<std::string> x {p0};
                auto y = str::join(x.begin(), x.end(), sep);
                INFO("Joined string: \"" + y + "\"");
                CHECK(y == p0);
            }

            {
                std::vector<std::string> x {p0, p1};
                auto y = str::join(x.begin(), x.end(), sep);
                INFO("Joined string: \"" + y + "\"");
                CHECK(y == (p0 + sep + p1));
            }

            {
                std::vector<std::string> x {p0, p1, p2};
                auto y = str::join(x.begin(), x.end(), sep);
                INFO("Joined string: \"" + y + "\"");
                CHECK(y == (p0 + sep + p1 + sep + p2));
            }

            {
                std::vector<std::string> x {{}, p0, p1, p2};
                auto y = str::join(x.begin(), x.end(), sep);
                INFO("Joined string: \"" + y + "\"");
                CHECK(y == (sep + p0 + sep + p1 + sep + p2));
            }

            {
                std::vector<std::string> x {p0, {}, p1, p2};
                auto y = str::join(x.begin(), x.end(), sep);
                INFO("Joined string: \"" + y + "\"");
                CHECK(y == (p0 + sep + sep + p1 + sep + p2));
            }

            {
                std::vector<std::string> x {p0, p1, p2, {}};
                auto y = str::join(x.begin(), x.end(), sep);
                INFO("Joined string: \"" + y + "\"");
                CHECK(y == (p0 + sep + p1 + sep + p2 + sep));
            }

            {
                std::vector<std::string> x {{}, {}, p0, p1, {}, {}, p2, {}, {}};
                auto y = str::join(x.begin(), x.end(), sep);
                INFO("Joined string: \"" + y + "\"");
                CHECK(y == (sep + sep + p0 + sep + p1 + sep + sep + sep + p2 + sep + sep));
            }
        }
    }

    SECTION("Test split and join with char separator")
    {
        const std::string p0{"abc"s};
        const std::string p1{"def"s};
        const std::string p2{"ghijkl"s};

        const std::vector<char> vsep = {
            ' ',
            ':',
            ';',
#if defined (ENABLE_BROKEN_TESTS) || !defined(__APPLE__)
            'A',
#endif
            '!',
            '|',
            '^',
            '$',
            '|',
            '(',
            ')',
            '\\',
            '\'',
            '"',
            '[',
            ']',
            '\n',
            '\t',
        };
        for (auto separator : vsep) {

            INFO("Testing separator: \"" + std::string{separator} + "\"");

            {
                std::string path;
                INFO("Splitting: \"" + path + "\"");
                auto x = str::split(path, separator);
                CHECK(x.empty());

                auto y = str::join(x.begin(), x.end(), {separator});
                INFO("Joined string: \"" + y + "\"");
                CHECK(y == path);
            }

            {
                // vector of one empty string element are converted to empty string
                std::vector<std::string> x { {} };
                auto y = str::join(x.begin(), x.end(), {separator});
                INFO("Joined string: \"" + y + "\"");
                CHECK(y.empty());
            }

            {
                std::string path {separator};
                INFO("Splitting: \"" + path + "\"");
                auto x = str::split(path, separator);
                CHECK(x.size() == 2);
                CHECK(x[0].empty());
                CHECK(x[1].empty());

                auto y = str::join(x.begin(), x.end(), {separator});
                INFO("Joined string: \"" + y + "\"");
                CHECK(y == path);
            }

            {
                std::string path {
                    std::string{separator} +
                    separator};
                INFO("Splitting: \"" + path + "\"");
                auto x = str::split(path, separator);
                CHECK(x.size() == 3);
                CHECK(x[0].empty());
                CHECK(x[1].empty());
                CHECK(x[2].empty());

                auto y = str::join(x.begin(), x.end(), {separator});
                INFO("Joined string: \"" + y + "\"");
                CHECK(y == path);
            }

            {
                const std::string& path {p0};
                INFO("Splitting: \"" + path + "\"");
                auto x = str::split(path, separator);
                CHECK(x.size() == 1);
                CHECK(x[0] == p0);

                auto y = str::join(x.begin(), x.end(), {separator});
                INFO("Joined string: \"" + y + "\"");
                CHECK(y == path);
            }

            {
                std::string path {
                    p0 +
                    separator +
                    p1};
                INFO("Splitting: \"" + path + "\"");
                auto x = str::split(path, separator);
                CHECK(x.size() == 2);
                CHECK(x[0] == p0);
                CHECK(x[1] == p1);

                auto y = str::join(x.begin(), x.end(), {separator});
                INFO("Joined string: \"" + y + "\"");
                CHECK(y == path);
            }

            {
                std::string path {
                    p0 +
                    separator +
                    p1 +
                    separator +
                    p2};
                INFO("Splitting: \"" + path + "\"");
                auto x = str::split(path, separator);
                CHECK(x.size() == 3);
                CHECK(x[0] == p0);
                CHECK(x[1] == p1);
                CHECK(x[2] == p2);

                auto y = str::join(x.begin(), x.end(), {separator});
                INFO("Joined string: \"" + y + "\"");
                CHECK(y == path);
            }

            {
                std::string path {
                    separator +
                    p0 +
                    separator +
                    p1 +
                    separator +
                    p2};
                INFO("Splitting: \"" + path + "\"");
                auto x = str::split(path, separator);
                CHECK(x.size() == 4);
                CHECK(x[0].empty());
                CHECK(x[1] == p0);
                CHECK(x[2] == p1);
                CHECK(x[3] == p2);

                auto y = str::join(x.begin(), x.end(), {separator});
                INFO("Joined string: \"" + y + "\"");
                CHECK(y == path);
            }

            {
                std::string path {
                    p0 +
                    separator +
                    separator +
                    p1 +
                    separator +
                    p2};
                INFO("Splitting: \"" + path + "\"");
                auto x = str::split(path, separator);
                CHECK(x.size() == 4);
                CHECK(x[0] == p0);
                CHECK(x[1].empty());
                CHECK(x[2] == p1);
                CHECK(x[3] == p2);

                auto y = str::join(x.begin(), x.end(), {separator});
                INFO("Joined string: \"" + y + "\"");
                CHECK(y == path);
            }

            {
                std::string path {
                    p0 +
                    separator +
                    p1 +
                    separator +
                    p2 +
                    separator};
                INFO("Splitting: \"" + path + "\"");
                auto x = str::split(path, separator);
                CHECK(x.size() == 4);
                CHECK(x[0] == p0);
                CHECK(x[1] == p1);
                CHECK(x[2] == p2);
                CHECK(x[3].empty());

                auto y = str::join(x.begin(), x.end(), {separator});
                INFO("Joined string: \"" + y + "\"");
                CHECK(y == path);
            }

            {
                std::string path {
                    std::string{separator} +
                    separator +
                    p0 +
                    separator +
                    p1 +
                    separator +
                    separator +
                    separator +
                    p2 +
                    separator +
                    separator};
                INFO("Splitting: \"" + path + "\"");
                auto x = str::split(path, separator);
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

                auto y = str::join(x.begin(), x.end(), {separator});
                INFO("Joined string: \"" + y + "\"");
                CHECK(y == path);
            }
        }
    }


    SECTION("Test split and join with string separator")
    {
        const std::string p0{"abc"s};
        const std::string p1{"def"s};
        const std::string p2{"ghijkl"s};

        std::vector<std::string> vsep
        {
#if defined (ENABLE_BROKEN_TESTS) || !defined(__APPLE__)
            "---"s,
            "-+"s,
            "^"s,
            "("s,
            "$"s,
            "[$^]"s,
#endif
        };

        for (const auto& separator : vsep)
        {
            INFO("Testing separator: \"" + separator + "\"");

            {
                std::string path;
                INFO("Splitting: \"" + path + "\"");
                auto x = str::split(path, separator);
                CHECK(x.empty());

                auto y = str::join(x.begin(), x.end(), {separator});
                INFO("Joined string: \"" + y + "\"");
                CHECK(y == path);
            }

            {
                // vector of one empty string element are converted to empty string
                std::vector<std::string> x { {} };
                auto y = str::join(x.begin(), x.end(), {separator});
                INFO("Joined string: \"" + y + "\"");
                CHECK(y.empty());
            }

            {
                std::string path {separator};
                INFO("Splitting: \"" + path + "\"");
                auto x = str::split(path, separator);
                CHECK(x.size() == 2);
                CHECK(x[0].empty());
                CHECK(x[1].empty());

                auto y = str::join(x.begin(), x.end(), {separator});
                INFO("Joined string: \"" + y + "\"");
                CHECK(y == path);
            }

            {
                std::string path {
                    std::string{separator} +
                    separator};
                INFO("Splitting: \"" + path + "\"");
                auto x = str::split(path, separator);
                CHECK(x.size() == 3);
                CHECK(x[0].empty());
                CHECK(x[1].empty());
                CHECK(x[2].empty());

                auto y = str::join(x.begin(), x.end(), {separator});
                INFO("Joined string: \"" + y + "\"");
                CHECK(y == path);
            }

            {
                const std::string& path {p0};
                INFO("Splitting: \"" + path + "\"");
                auto x = str::split(path, separator);
                CHECK(x.size() == 1);
                CHECK(x[0] == p0);

                auto y = str::join(x.begin(), x.end(), {separator});
                INFO("Joined string: \"" + y + "\"");
                CHECK(y == path);
            }

            {
                std::string path {
                    p0 +
                    separator +
                    p1};
                INFO("Splitting: \"" + path + "\"");
                auto x = str::split(path, separator);
                CHECK(x.size() == 2);
                CHECK(x[0] == p0);
                CHECK(x[1] == p1);

                auto y = str::join(x.begin(), x.end(), {separator});
                INFO("Joined string: \"" + y + "\"");
                CHECK(y == path);
            }

            {
                std::string path {
                    p0 +
                    separator +
                    p1 +
                    separator +
                    p2};
                INFO("Splitting: \"" + path + "\"");
                auto x = str::split(path, separator);
                CHECK(x.size() == 3);
                CHECK(x[0] == p0);
                CHECK(x[1] == p1);
                CHECK(x[2] == p2);

                auto y = str::join(x.begin(), x.end(), {separator});
                INFO("Joined string: \"" + y + "\"");
                CHECK(y == path);
            }

            {
                std::string path {
                    separator +
                    p0 +
                    separator +
                    p1 +
                    separator +
                    p2};
                INFO("Splitting: \"" + path + "\"");
                auto x = str::split(path, separator);
                CHECK(x.size() == 4);
                CHECK(x[0].empty());
                CHECK(x[1] == p0);
                CHECK(x[2] == p1);
                CHECK(x[3] == p2);

                auto y = str::join(x.begin(), x.end(), {separator});
                INFO("Joined string: \"" + y + "\"");
                CHECK(y == path);
            }

            {
                std::string path {
                    p0 +
                    separator +
                    separator +
                    p1 +
                    separator +
                    p2};
                INFO("Splitting: \"" + path + "\"");
                auto x = str::split(path, separator);
                CHECK(x.size() == 4);
                CHECK(x[0] == p0);
                CHECK(x[1].empty());
                CHECK(x[2] == p1);
                CHECK(x[3] == p2);

                auto y = str::join(x.begin(), x.end(), {separator});
                INFO("Joined string: \"" + y + "\"");
                CHECK(y == path);
            }

            {
                std::string path {
                    p0 +
                    separator +
                    p1 +
                    separator +
                    p2 +
                    separator};
                INFO("Splitting: \"" + path + "\"");
                auto x = str::split(path, separator);
                CHECK(x.size() == 4);
                CHECK(x[0] == p0);
                CHECK(x[1] == p1);
                CHECK(x[2] == p2);
                CHECK(x[3].empty());

                auto y = str::join(x.begin(), x.end(), {separator});
                INFO("Joined string: \"" + y + "\"");
                CHECK(y == path);
            }

            {
                std::string path {
                    std::string{separator} +
                    separator +
                    p0 +
                    separator +
                    p1 +
                    separator +
                    separator +
                    separator +
                    p2 +
                    separator +
                    separator};
                INFO("Splitting: \"" + path + "\"");
                auto x = str::split(path, separator);
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

                auto y = str::join(x.begin(), x.end(), {separator});
                INFO("Joined string: \"" + y + "\"");
                CHECK(y == path);
            }
        }
    }


    SECTION("Test split with regex separator")
    {
        {
            const std::string str{"TeSt sPlIt wItH ReGeX SePaRaToR"};
#if !defined(WIN32)
            const std::regex regex{"[aeiou]", std::regex::ECMAScript|std::regex::icase};
#else
            // At the time of writing this test, std::regex::icase does not work
            // in the same way on visual studio, I don't know which behaviour
            // is correct
            const std::regex regex{"[aeiouAEIOU]", std::regex::ECMAScript};
#endif
            auto x = str::split(str, regex);
            CHECK(x.size() == 10);
            CHECK(x[0] == "T");
            CHECK(x[1] == "St sPl");
            CHECK(x[2] == "t w");
            CHECK(x[3] == "tH R");
            CHECK(x[4] == "G");
            CHECK(x[5] == "X S");
            CHECK(x[6] == "P");
            CHECK(x[7] == "R");
            CHECK(x[8] == "T");
            CHECK(x[9] == "R");
        }

        {
            const std::string str{"TeSt \"sPlIt wItH\" ReGeX\n SePaRaToR\t..."};
            const std::regex regex{"[\" \t\n]+"};
            auto x = str::split(str, regex);
            CHECK(x.size() == 6);
            CHECK(x[0] == "TeSt");
            CHECK(x[1] == "sPlIt");
            CHECK(x[2] == "wItH");
            CHECK(x[3] == "ReGeX");
            CHECK(x[4] == "SePaRaToR");
            CHECK(x[5] == "...");
        }
    }
}
