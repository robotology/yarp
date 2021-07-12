/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/os/ResourceFinder.h>

#include <yarp/conf/dirs.h>
#include <yarp/conf/environment.h>
#include <yarp/conf/filesystem.h>

#include <yarp/os/Network.h>
#include <yarp/os/Os.h>
#include <yarp/os/YarpPlugin.h>

#include <cstdlib>
#include <cstdio>

#include <catch.hpp>
#include <harness.h>

using namespace yarp::os;
using namespace yarp::os::impl;

static yarp::os::Bottle env;

static void saveEnvironment(const char *key)
{
    bool found = false;
    std::string val = yarp::conf::environment::get_string(key, &found);
    Bottle& lst = env.addList();
    lst.addString(key);
    lst.addString(val);
    lst.addInt32(found?1:0);
}

static void restoreEnvironment()
{
    for (size_t i=0; i<env.size(); i++) {
        Bottle *lst = env.get(i).asList();
        if (lst==nullptr) continue;
        std::string key = lst->get(0).asString();
        std::string val = lst->get(1).asString();
        bool found = lst->get(2).asInt32()?true:false;
        if (!found) {
            yarp::conf::environment::unset(key);
        } else {
            yarp::conf::environment::set_string(key, val);
        }
    }
    env.clear();
}

static void mkdir(const std::string& dirname)
{
    if (yarp::os::stat(dirname.c_str())<0) {
        yarp::os::mkdir(dirname.c_str());
    }
    int r = yarp::os::stat(dirname.c_str());
    REQUIRE(r>=0); // test directory present}
}

static std::string pathify(const Bottle& dirs)
{
    char buf[1000];
    char *result = yarp::os::getcwd(buf, sizeof(buf));
    if (!result) {
        REQUIRE(result!=nullptr); // cwd/pwd not too long
    }
    std::string slash = std::string{yarp::conf::filesystem::preferred_separator};
    std::string dir = buf;
    for (size_t i=0; i<dirs.size(); i++) {
        dir += slash;
        dir = dir + dirs.get(i).asString();
    }
    return dir;
}

static void mkdir(const Bottle& dirs)
{
    std::string slash = std::string{yarp::conf::filesystem::preferred_separator};
    std::string dir = "";
    for (size_t i=0; i<dirs.size(); i++) {
        if (i>0) dir += slash;
        dir = dir + dirs.get(i).asString();
        mkdir(dir);
    }
}

static void setUpTestArea(bool etc_pathd)
{
    std::string colon = std::string{yarp::conf::environment::path_separator};
    std::string slash = std::string{yarp::conf::filesystem::preferred_separator};
    FILE *fout;

    std::string base = etc_pathd ? "__test_dir_rf_a1" : "__test_dir_rf_a2";
    Bottle yarp_data_home;
    yarp_data_home.addString(base);
    yarp_data_home.addString("home");
    yarp_data_home.addString("yarper");
    yarp_data_home.addString(".local");
    yarp_data_home.addString("share");
    yarp_data_home.addString("yarp");
    mkdir(yarp_data_home);

    Bottle yarp_data_home_shadow;
    yarp_data_home_shadow.addString(base);
    yarp_data_home_shadow.addString("home");
    yarp_data_home_shadow.addString("yarper");
    yarp_data_home_shadow.addString(".local");
    yarp_data_home_shadow.addString("share");
    yarp_data_home_shadow.addString("yarp");
    yarp_data_home_shadow.addString("contexts");
    yarp_data_home_shadow.addString("shadowtest");
    mkdir(yarp_data_home_shadow);

    Bottle yarp_config_home;
    yarp_config_home.addString(base);
    yarp_config_home.addString("home");
    yarp_config_home.addString("yarper");
    yarp_config_home.addString(".config");
    yarp_config_home.addString("yarp");
    mkdir(yarp_config_home);

    Bottle yarp_config_home_plugins;
    yarp_config_home_plugins.addString(base);
    yarp_config_home_plugins.addString("home");
    yarp_config_home_plugins.addString("yarper");
    yarp_config_home_plugins.addString(".config");
    yarp_config_home_plugins.addString("yarp");
    yarp_config_home_plugins.addString("plugins");
    mkdir(yarp_config_home_plugins);

    Bottle yarp_data_dir0;
    yarp_data_dir0.addString(base);
    yarp_data_dir0.addString("usr");
    yarp_data_dir0.addString("share");
    yarp_data_dir0.addString("yarp");
    mkdir(yarp_data_dir0);

    Bottle yarp_data_dir0_plugins;
    yarp_data_dir0_plugins.addString(base);
    yarp_data_dir0_plugins.addString("usr");
    yarp_data_dir0_plugins.addString("share");
    yarp_data_dir0_plugins.addString("yarp");
    yarp_data_dir0_plugins.addString("plugins");
    mkdir(yarp_data_dir0_plugins);

    Bottle yarp_context_dir;
    yarp_context_dir.addString(base);
    yarp_context_dir.addString("usr");
    yarp_context_dir.addString("share");
    yarp_context_dir.addString("yarp");
    yarp_context_dir.addString("contexts");
    yarp_context_dir.addString("my_app");
    mkdir(yarp_context_dir);

    Bottle yarp_context_dir2;
    yarp_context_dir2.addString(base);
    yarp_context_dir2.addString("usr");
    yarp_context_dir2.addString("share");
    yarp_context_dir2.addString("yarp");
    yarp_context_dir2.addString("contexts");
    yarp_context_dir2.addString("shadowtest");
    mkdir(yarp_context_dir2);

    Bottle yarp_data_dir1;
    yarp_data_dir1.addString(base);
    yarp_data_dir1.addString("usr");
    yarp_data_dir1.addString("local");
    yarp_data_dir1.addString("share");
    yarp_data_dir1.addString("yarp");
    // do not make this

    Bottle yarp_config_dir0;
    yarp_config_dir0.addString(base);
    yarp_config_dir0.addString("etc");
    yarp_config_dir0.addString("yarp");
    mkdir(yarp_config_dir0);

    Bottle pathd;
    if (etc_pathd) {
        pathd = yarp_config_dir0;
    } else {
        pathd = yarp_data_dir0;
    }
    pathd.addString("config");
    pathd.addString("path.d");
    mkdir(pathd);

    Bottle project1;
    project1.addString(base);
    project1.addString("usr");
    project1.addString("share");
    project1.addString("project1");
    mkdir(project1);

    Bottle project2;
    project2.addString(base);
    project2.addString("usr");
    project2.addString("share");
    project2.addString("project2");
    mkdir(project2);

    Bottle path_project1;
    path_project1.addString("path");
    path_project1.addString(pathify(project1));

    Bottle path_project2;
    path_project2.addString("path");
    path_project2.addString(pathify(project2));

    fout = fopen((pathify(pathd)+slash+"project1.ini").c_str(), "w");
    REQUIRE(fout!=nullptr);
    fprintf(fout, "[search project1]\n");
    fprintf(fout, "%s\n", path_project1.toString().c_str());
    fclose(fout);
    fout = nullptr;

    fout = fopen((pathify(pathd)+slash+"project2.ini").c_str(), "w");
    REQUIRE(fout!=nullptr);
    fprintf(fout, "[search project2]\n");
    fprintf(fout, "%s\n", path_project2.toString().c_str());
    fclose(fout);
    fout = nullptr;

    saveEnvironment("YARP_DATA_HOME");
    saveEnvironment("YARP_CONFIG_HOME");
    saveEnvironment("YARP_DATA_DIRS");
    saveEnvironment("YARP_CONFIG_DIRS");
    saveEnvironment("YARP_ROBOT_NAME");

    yarp::conf::environment::set_string("YARP_DATA_HOME", pathify(yarp_data_home));
    yarp::conf::environment::set_string("YARP_CONFIG_HOME", pathify(yarp_config_home));
    yarp::conf::environment::set_string("YARP_DATA_DIRS",
                            pathify(yarp_data_dir0) +
                            colon +
                            pathify(yarp_data_dir1));
    yarp::conf::environment::set_string("YARP_CONFIG_DIRS", pathify(yarp_config_dir0));
    yarp::conf::environment::set_string("YARP_ROBOT_NAME", "dummyRobot");


    fout = fopen((pathify(yarp_data_home)+slash+"data.ini").c_str(), "w");
    REQUIRE(fout!=nullptr);
    fprintf(fout, "magic_number = 42\n");
    fprintf(fout, "[data_home]\n");
    fprintf(fout, "x = 2\n");
    fclose(fout);
    fout = nullptr;

    fout = fopen((pathify(yarp_data_dir0)+slash+"data.ini").c_str(), "w");
    REQUIRE(fout!=nullptr);
    fprintf(fout, "magic_number = 22\n");
    fprintf(fout, "[data_dir0]\n");
    fprintf(fout, "x = 3\n");
    fclose(fout);
    fout = nullptr;

    fout = fopen((pathify(project1)+slash+"data.ini").c_str(), "w");
    REQUIRE(fout!=nullptr);
    fprintf(fout, "magic_number = 101\n");
    fprintf(fout, "[project1]\n");
    fprintf(fout, "x = 3\n");
    fclose(fout);
    fout = nullptr;

    fout = fopen((pathify(yarp_context_dir)+slash+"my_app.ini").c_str(), "w");
    REQUIRE(fout!=nullptr);
    fprintf(fout, "magic_number = 1000\n");
    fclose(fout);
    fout = nullptr;

    fout = fopen((pathify(yarp_context_dir2)+slash+"shadow.ini").c_str(), "w");
    REQUIRE(fout!=nullptr);
    fprintf(fout, "magic_number = 5000\n");
    fclose(fout);
    fout = nullptr;

    fout = fopen((pathify(yarp_data_home_shadow)+slash+"shadow.ini").c_str(), "w");
    REQUIRE(fout!=nullptr);
    fprintf(fout, "magic_number = 5001\n");
    fclose(fout);
    fout = nullptr;

    fout = fopen((pathify(yarp_context_dir2)+slash+"noshadow.ini").c_str(), "w");
    REQUIRE(fout!=nullptr);
    fprintf(fout, "magic_number = 5002\n");
    fclose(fout);
    fout = nullptr;

    fout = fopen((pathify(yarp_config_home_plugins)+slash+"fakedev1.ini").c_str(), "w");
    REQUIRE(fout!=nullptr);
    fprintf(fout, "[plugin fakedev1]\n");
    fprintf(fout, "type device\n");
    fprintf(fout, "name fakedev1\n");
    fprintf(fout, "library yarp_fakedev1\n");
    fprintf(fout, "part fakedev1\n");
    fclose(fout);
    fout = nullptr;

    fout = fopen((pathify(yarp_data_dir0_plugins)+slash+"fakedev2.ini").c_str(), "w");
    REQUIRE(fout!=nullptr);
    fprintf(fout, "[plugin fakedev2]\n");
    fprintf(fout, "type device\n");
    fprintf(fout, "name fakedev2\n");
    fprintf(fout, "library yarp_fakedev2\n");
    fprintf(fout, "part fakedev2\n");
    fclose(fout);
    fout = nullptr;
}

static void breakDownTestArea()
{
    restoreEnvironment();
}

TEST_CASE("os::ResourceFinderTest", "[yarp::os]")
{
    SECTION("testing the basics of RF...")
    {
        ResourceFinder rf;

        const char *fname0 = "_yarp_regression_test.ini";
        const char *fname1 = "_yarp_regression_test_rf1.txt";
        const char *fname2 = "_yarp_regression_test_rf2.txt";

        // create some test files

        {
            FILE *fout = fopen(fname0, "w");
            REQUIRE(fout!=nullptr);
            fprintf(fout, "style capability\n");
            fprintf(fout, "capability_directory \".\"\n");
            fprintf(fout, "default_capability \".\"\n");
            fclose(fout);
            fout = nullptr;

            fout = fopen(fname1, "w");
            REQUIRE(fout!=nullptr);
            fprintf(fout, "alt %s\n", fname2);
            fclose(fout);
            fout = nullptr;

            fout = fopen(fname2, "w");
            fprintf(fout, "x 14\n");
            fclose(fout);
            fout = nullptr;

            const char *argv[] = { "ignore",
                                   "--_yarp_regression_test", ".",
                                   "--from", fname1,
                                   "--verbose", "0",
                                   nullptr };
            int argc = 7;

            rf.configure(argc, (char **)argv);
            std::string alt = rf.findFile("alt");
            CHECK(alt!=""); // found ini file

            rf.setDefault("alt2", fname2);
            alt = rf.findFile("alt2");
            CHECK(alt!=""); // default setting worked

            rf.setDefault("alt3", "_yarp_nonexistent.txt");
            alt = rf.findFile("alt3");
            CHECK(alt==""); // cannot find nonexistent files

            rf.setDefault("alt", "_yarp_nonexistent.txt");
            alt = rf.findFile("alt");
            CHECK(alt!=""); // default setting is safe

            CHECK(rf.findPath()!=""); // existing path found

            alt=rf.findFileByName(fname1);
            CHECK(alt!=""); // found file by name
        }
    }

    SECTION("make sure command line args take priority...")
    {

        const char *fname0 = "_yarp_regression_test.ini";
        const char *fname1 = "_yarp_regression_test_rf1.txt";

        // create some test files

        {
            FILE *fout = fopen(fname0, "w");
            REQUIRE(fout!=nullptr);
            fprintf(fout, "style capability\n");
            fprintf(fout, "capability_directory \".\"\n");
            fprintf(fout, "default_capability \".\"\n");
            fclose(fout);
            fout = nullptr;

            fout = fopen(fname1, "w");
            fprintf(fout, "x 14\n");
            fclose(fout);
            fout = nullptr;

            const char *argv[] = { "ignore",
                                   "--_yarp_regression_test", ".",
                                   "--from", fname1,
                                   "--verbose", "0",
                                   nullptr };
            int argc = 7;

            ResourceFinder rf1;
            rf1.configure(argc, (char **)argv);
            CHECK(rf1.find("x").asInt32() == 14); // found x

            const char *argv2[] = { "ignore",
                                    "--_yarp_regression_test", ".",
                                    "--from", fname1,
                                    "--verbose", "0",
                                    "--x", "20",
                                    "--y", "30",
                                    nullptr };
            int argc2 = 11;

            ResourceFinder rf2;
            rf2.configure(argc2, (char **)argv2);
            CHECK(rf2.find("y").asInt32() == 30); // found y
            CHECK(rf2.find("x").asInt32() == 20); // override x
        }
    }

    SECTION("test context setting")
    {
        ResourceFinder rf;
        const char *argv[] = { "ignore",
                               "--_yarp_regression_test", ".",
                               "--context", "zig",
                               "--verbose", "0",
                               nullptr };
        int argc = 7;
        rf.configure(argc, (char **)argv);
        CHECK(rf.getContext() == "zig"); // recovered context
    }

    SECTION("test subgroup")
    {
        const char *fname0 = "_yarp_regression_subgroup_test.ini";
        const char *fname1 = "_yarp_regression_subgroup_test_rf1.txt";
        const char *fname2 = "_yarp_regression_subgroup_test_rf2.txt";
        {
            FILE *fout = fopen(fname0, "w");
            REQUIRE(fout!=nullptr);
            fprintf(fout, "[section1]\n");
            fprintf(fout, "fname \"_yarp_regression_subgroup_test_rf1.txt\"\n");
            fprintf(fout, "[section2]\n");
            fprintf(fout, "fname \"_yarp_regression_subgroup_test_rf2.txt\"\n");
            fclose(fout);
            fout = nullptr;
        }
        {
            FILE *fout = fopen(fname1, "w");
            REQUIRE(fout!=nullptr);
            fprintf(fout, "x 1\n");
            fclose(fout);
            fout = nullptr;
        }
        {
            FILE *fout = fopen(fname2, "w");
            REQUIRE(fout!=nullptr);
            fprintf(fout, "x 2\n");
            fclose(fout);
            fout = nullptr;
        }
        ResourceFinder rf;
        const char *argv[] = { "ignore",
                               "--_yarp_regression_test", ".",
                               "--from", fname0,
                               "--verbose", "0",
                               nullptr };
        int argc = 7;
        rf.configure(argc, (char **)argv);
        ResourceFinder rf1 = rf.findNestedResourceFinder("section1");
        //CHECK(rf1.findFile("fname").c_str() == fname1); // section1 ok
        CHECK_FALSE(rf1.isNull()); // section1 not null ok
        ResourceFinder rf2 = rf.findNestedResourceFinder("section2");
        //CHECK(rf2.findFile("fname").c_str() == fname2); // section2 ok
        CHECK_FALSE(rf2.isNull()); // section2 not null ok
        ResourceFinder rf3 = rf.findNestedResourceFinder("section3");
        CHECK(rf3.isNull()); // section3 null ok
    }

    SECTION("test default values of generic type")
    {
        int defInt=42;
        double defDouble=42.42;
        std::string defString="fortytwo";
        Bottle defList("(answers (42 24))");
        const char *argv[] = { nullptr };
        int argc = 1;
        ResourceFinder rf;
        rf.setDefault("int", defInt);
        rf.setDefault("double", defDouble);
        rf.setDefault("string", defString);
        rf.setDefault("constchar", defString.c_str());
        rf.setDefault("list", defList.toString());
        rf.configure(argc, (char **)argv);
        CHECK(rf.find("int").asInt32() == defInt); // default integer set correctly
        CHECK(rf.find("double").asFloat64() == Approx(defDouble)); // default double set correctly
        CHECK(rf.find("string").asString() == defString); // default string set correctly
        CHECK(rf.find("constchar").asString() == defString); // default string (passed as const char*) set correctly
        Bottle *foundList=rf.find("list").asList();
        REQUIRE(foundList!=nullptr);
        CHECK(rf.find("list").asList()->get(0).asString() == "answers"); // default list set correctly
    }

#ifndef YARP_NO_DEPRECATED // Since YARP 3.5
YARP_WARNING_PUSH
YARP_DISABLE_DEPRECATED_WARNING
#if !defined(_WIN32) && !defined(__APPLE__)
    SECTION("test getDataHome")
    {
        saveEnvironment("YARP_DATA_HOME");
        saveEnvironment("XDG_DATA_HOME");
        saveEnvironment("HOME");
        yarp::conf::environment::set_string("YARP_DATA_HOME", "/foo");
        CHECK(ResourceFinder::getDataHome() == "/foo"); // YARP_DATA_HOME noticed
        yarp::conf::environment::unset("YARP_DATA_HOME");
        yarp::conf::environment::set_string("XDG_DATA_HOME", "/foo");
        std::string slash = std::string{yarp::conf::filesystem::preferred_separator};
        CHECK(ResourceFinder::getDataHome() == (std::string("/foo") + slash + "yarp")); // XDG_DATA_HOME noticed
        yarp::conf::environment::unset("XDG_DATA_HOME");
        yarp::conf::environment::set_string("HOME", "/foo");
        CHECK(ResourceFinder::getDataHome() == "/foo/.local/share/yarp"); // HOME noticed
        restoreEnvironment();
    }

    SECTION("test getConfigHome")
    {
        saveEnvironment("YARP_CONFIG_HOME");
        saveEnvironment("XDG_CONFIG_HOME");
        saveEnvironment("HOME");
        yarp::conf::environment::set_string("YARP_CONFIG_HOME", "/foo");
        CHECK(ResourceFinder::getConfigHome() == "/foo"); // YARP_CONFIG_HOME noticed
        yarp::conf::environment::unset("YARP_CONFIG_HOME");
        yarp::conf::environment::set_string("XDG_CONFIG_HOME", "/foo");
        std::string slash = std::string{yarp::conf::filesystem::preferred_separator};
        CHECK(ResourceFinder::getConfigHome() == (std::string("/foo") + slash + "yarp")); // XDG_CONFIG_HOME noticed
        yarp::conf::environment::unset("XDG_CONFIG_HOME");
        yarp::conf::environment::set_string("HOME", "/foo");
        CHECK(ResourceFinder::getConfigHome() == "/foo/.config/yarp"); // HOME noticed
        restoreEnvironment();
    }

    SECTION("test getDataDirs")
    {
        saveEnvironment("YARP_DATA_DIRS");
        saveEnvironment("XDG_DATA_DIRS");
        std::string slash = std::string{yarp::conf::filesystem::preferred_separator};
        std::string colon = std::string{yarp::conf::environment::path_separator};
        std::string foobar = std::string("/foo") + colon + "/bar";
        std::string yfoo = std::string("/foo") + slash + "yarp";
        std::string ybar = std::string("/bar") + slash + "yarp";
        yarp::conf::environment::set_string("YARP_DATA_DIRS", foobar);
        Bottle dirs;
        dirs = ResourceFinder::getDataDirs();
        CHECK(dirs.size() == (size_t) 2); // YARP_DATA_DIRS parsed as two directories
        CHECK(dirs.get(0).asString() == "/foo"); // YARP_DATA_DIRS first dir ok
        CHECK(dirs.get(1).asString() == "/bar"); // YARP_DATA_DIRS second dir ok

        yarp::conf::environment::set_string("YARP_DATA_DIRS", "/foo");
        dirs = ResourceFinder::getDataDirs();
        CHECK(dirs.size() == (size_t) 1); // YARP_DATA_DIRS parsed as one directory

        yarp::conf::environment::unset("YARP_DATA_DIRS");
        yarp::conf::environment::set_string("XDG_DATA_DIRS", foobar);
        dirs = ResourceFinder::getDataDirs();
        CHECK(dirs.size() == (size_t) 2); // XDG_DATA_DIRS gives two directories
        CHECK(dirs.get(0).asString() == yfoo); // XDG_DATA_DIRS first dir ok
        CHECK(dirs.get(1).asString() == ybar); // XDG_DATA_DIRS second dir ok

        yarp::conf::environment::unset("XDG_DATA_DIRS");
        dirs = ResourceFinder::getDataDirs();
        CHECK(dirs.size() == (size_t) 2); // DATA_DIRS default length 2
        CHECK(dirs.get(0).asString() == "/usr/local/share/yarp"); // DATA_DIRS default element 0 is ok
        CHECK(dirs.get(1).asString() == "/usr/share/yarp"); // DATA_DIRS default element 1 is ok

        restoreEnvironment();
    }

    SECTION("test getConfigDirs")
    {
        saveEnvironment("YARP_CONFIG_DIRS");
        saveEnvironment("XDG_CONFIG_DIRS");
        std::string slash = std::string{yarp::conf::filesystem::preferred_separator};
        std::string colon = std::string{yarp::conf::environment::path_separator};
        std::string foobar = std::string("/foo") + colon + "/bar";
        std::string yfoo = std::string("/foo") + slash + "yarp";
        std::string ybar = std::string("/bar") + slash + "yarp";
        yarp::conf::environment::set_string("YARP_CONFIG_DIRS", foobar);
        Bottle dirs;
        dirs = ResourceFinder::getConfigDirs();
        CHECK(dirs.size() == (size_t) 2); // YARP_CONFIG_DIRS parsed as two directories
        CHECK(dirs.get(0).asString() == "/foo"); // YARP_CONFIG_DIRS first dir ok
        CHECK(dirs.get(1).asString() == "/bar"); // YARP_CONFIG_DIRS second dir ok

        yarp::conf::environment::unset("YARP_CONFIG_DIRS");
        yarp::conf::environment::set_string("XDG_CONFIG_DIRS", foobar);
        dirs = ResourceFinder::getConfigDirs();
        CHECK(dirs.size() == (size_t) 2); // XDG_CONFIG_DIRS gives two directories
        CHECK(dirs.get(0).asString() == yfoo); // XDG_CONFIG_DIRS first dir ok
        CHECK(dirs.get(1).asString() == ybar); // XDG_CONFIG_DIRS second dir ok

        yarp::conf::environment::unset("XDG_CONFIG_DIRS");
        dirs = ResourceFinder::getConfigDirs();
        CHECK(dirs.size() == (size_t) 1); // CONFIG_DIRS default length 1
        CHECK(dirs.get(0).asString() == "/etc/xdg/yarp"); // CONFIG_DIRS default is ok

        restoreEnvironment();
    }
#endif // !defined(_WIN32) && !defined(__APPLE__)
YARP_WARNING_POP
#endif // YARP_NO_DEPRECATED

    SECTION("test readConfig")
    {
        for (int area=1; area<2; area++) {
            if (area==0) {
                // currently ruled out in spec, but may come back
                INFO("test readConfig with pathd in /etc/...");
            } else {
                INFO("test readConfig with pathd in /usr/share/yarp/...");
            }
            setUpTestArea(area==0);

            ResourceFinder rf;
            Property p;
            bool ok = rf.readConfig(p, "data.ini",
                                    ResourceFinderOptions::findFirstMatch());
            CHECK(ok); // read a data.ini
            CHECK(p.find("magic_number").asInt32() == 42); // right version found
            CHECK(p.check("data_home")); // data_home found
            CHECK_FALSE(p.check("data_dir0")); // data_dirs not found
            CHECK_FALSE(p.check("project1")); // project1 not found
            p.clear();
            rf.readConfig(p, "data.ini",
                          ResourceFinderOptions::findAllMatch());
            CHECK(p.find("magic_number").asInt32() == 42); // right priority
            CHECK(p.check("data_home")); // data_home found
            CHECK(p.check("data_dir0")); // data_dirs found
            CHECK(p.check("project1")); // project1 found

            breakDownTestArea();
        }
    }

    SECTION("test context version 2")
    {
        setUpTestArea(false);

        {
            ResourceFinder rf;
            rf.setDefaultContext("my_app");
            Property p;
            bool ok = rf.readConfig(p, "my_app.ini",
                                    ResourceFinderOptions::findFirstMatch());
            CHECK(ok); // read a my_app.ini
            CHECK(p.find("magic_number").asInt32() == 1000); // right version found
        }

        {
            ResourceFinder rf;
            rf.setDefaultContext("my_app");
            rf.setDefaultConfigFile("my_app.ini");
            rf.configure(0, nullptr);
            CHECK(rf.find("magic_number").asInt32() == 1000); // my_app.ini found as default config file
        }

        {
            ResourceFinder rf;
            std::string contextName = "my_app";
            rf.setDefaultContext(contextName);
            rf.setDefaultConfigFile("my_app.ini");
            rf.configure(0, nullptr);
            CHECK(rf.find("magic_number").asInt32() == 1000); // my_app.ini found as default config file
        }

        {
            ResourceFinder rf;
            std::string contextName = "my_app";
            rf.setDefaultContext(contextName.c_str());
            rf.setDefaultConfigFile("my_app.ini");
            rf.configure(0, nullptr);
            CHECK(rf.find("magic_number").asInt32() == 1000); // my_app.ini found as default config file
        }

        {
            ResourceFinder rf;
            rf.setDefaultContext("shadowtest");
            rf.setDefaultConfigFile("shadow.ini");
            rf.configure(0, nullptr);
            CHECK(rf.find("magic_number").asInt32() == 5001); // shadow.ini found as correct location
        }

        {
            ResourceFinder rf;
            rf.setDefaultContext("shadowtest");
            rf.setDefaultConfigFile("noshadow.ini");
            rf.configure(0, nullptr);
            CHECK(rf.find("magic_number").asInt32() == 5002); // noshadow.ini found as correct location
        }

        {
            ResourceFinder rf;
            rf.setDefaultContext("shadowtest");
            rf.setDefaultConfigFile("noshadow.ini");
            rf.configure(0, nullptr);
            CHECK(rf.find("magic_number").asInt32() == 5002); // noshadow.ini found as correct location
            Property p;
            ResourceFinderOptions opts;
            CHECK(rf.readConfig(p, "shadow.ini", opts)); // found shadow.ini
            CHECK(p.find("magic_number").asInt32() == 5001); // shadow.ini found as correct location
        }

        breakDownTestArea();
    }

    SECTION("test context version 2")
    {
        ResourceFinder rf1;
        rf1.setDefault("testNumber", "fortytwo");
        rf1.configure(0, nullptr);
        CHECK(rf1.find("testNumber").asString() == "fortytwo"); // Original RF finds the default value
        ResourceFinder rf2(rf1);
        CHECK(rf2.find("testNumber").asString() == "fortytwo"); // Copied RF finds the default passed to the original one
        ResourceFinder rf3;
        rf3=rf1;
        CHECK(rf3.find("testNumber").asString() == "fortytwo"); // Assigned RF finds the default passed to the original one
    }


    SECTION("test get 'home' dirs for writing")
    {
        std::string slash = std::string{yarp::conf::filesystem::preferred_separator};
        setUpTestArea(false);

        {
            ResourceFinder rf;
            const char *fname1 = "_yarp_regression_test_rf1.txt";
            rf.setDefaultContext("my_app");
            rf.setDefaultConfigFile(fname1); // should be in pwd
            rf.configure(0, nullptr);

            char buf[1000];
            char *result = yarp::os::getcwd(buf, sizeof(buf));
            CHECK(rf.getHomeContextPath() == result); // cwd found as context directory for writing
            CHECK(rf.getHomeRobotPath() == result); // cwd found as robot directory for writing
        }

        {
            ResourceFinder rf;
            rf.setDefaultContext("my_app");
            rf.setDefaultConfigFile("my_app.ini");
            rf.configure(0, nullptr);

            bool found;
            std::string robot = yarp::conf::environment::get_string("YARP_ROBOT_NAME", &found);
            if (!found) robot = "default";
            CHECK(rf.getHomeContextPath() == yarp::conf::dirs::yarpdatahome() + slash + "contexts" + slash + "my_app"); // $YARP_DATA_HOME/contexts/my_app found as directory for writing
            CHECK(rf.getHomeRobotPath() == yarp::conf::dirs::yarpdatahome() + slash + "robots" + slash + robot); // $YARP_DATA_HOME/robots/dummyRobot found as directory for writing

        }
        breakDownTestArea();
    }


    SECTION("test get 'home' dirs for writing")
    {
        setUpTestArea(false);
        YarpPluginSelector selector;
        selector.scan();
        Bottle lst = selector.getSelectedPlugins();
        CHECK(lst.check("fakedev1")); // first device present
        CHECK(lst.check("fakedev2")); // second device present
        CHECK_FALSE(lst.check("fakedev3")); // non-existent device absent
        breakDownTestArea();
    }

    SECTION("test fail behavior on --from / setDefaultConfigFile")
    {
        setUpTestArea(false);

        {
            ResourceFinder rf;
            rf.setDefaultContext("my_app");
            rf.setDefaultConfigFile("my_app.ini");
            bool configures = rf.configure(0, nullptr);
            CHECK(configures); // ok with default file that exists
        }

        {
            ResourceFinder rf;
            rf.setDefaultContext("my_app");
            const char *argv[] = { "ignore",
                                   "--from", "my_app.ini",
                                   nullptr };
            int argc = 3;
            bool configures = rf.configure(argc, (char **)argv);
            CHECK(configures); // ok with from file that exists
        }

        {
            ResourceFinder rf;
            rf.setDefaultContext("my_app");
            rf.setDefaultConfigFile("my_app_not_there.ini");
            bool configures = rf.configure(0, nullptr);
            CHECK(configures); // ok with default file that does not exist
        }

        {
            ResourceFinder rf;
            rf.setDefaultContext("my_app");
            const char *argv[] = { "ignore",
                                   "--from", "my_app_not_there.ini",
                                   nullptr };
            int argc = 3;
            bool configures = rf.configure(argc, (char **)argv);
            CHECK_FALSE(configures); // fails with from file that is missing
        }
    }
}
