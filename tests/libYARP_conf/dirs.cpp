/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/conf/environment.h>
#include <yarp/conf/dirs.h>

#include <tuple>
#include <vector>

#include <catch.hpp>
#include <harness.h>

// static std::map<std::string, std::string> env;

static std::vector<std::tuple<std::string, std::string, bool>> env;

static void saveEnvironment(const std::string& key)
{
    bool found = false;
    std::string val = yarp::conf::environment::get_string(key, &found);
    env.emplace_back(key, val, found);
    yarp::conf::environment::unset(key);
}

static void restoreEnvironment()
{
    for (const auto& var : env) {
        if (std::get<2>(var)) {
            yarp::conf::environment::set_string(std::get<0>(var), std::get<1>(var));
        } else {
            yarp::conf::environment::unset(std::get<0>(var));
        }
    }
    env.clear();
}


TEST_CASE("conf::dirs", "[yarp::conf]")
{
    saveEnvironment(yarp::conf::dirs::USER);
    saveEnvironment(yarp::conf::dirs::USERNAME);
    saveEnvironment(yarp::conf::dirs::HOME);
    saveEnvironment(yarp::conf::dirs::USERPROFILE);
    saveEnvironment(yarp::conf::dirs::TMP);
    saveEnvironment(yarp::conf::dirs::TEMP);
    saveEnvironment(yarp::conf::dirs::TMPDIR);
    saveEnvironment(yarp::conf::dirs::XDG_DATA_HOME);
    saveEnvironment(yarp::conf::dirs::XDG_DATA_DIRS);
    saveEnvironment(yarp::conf::dirs::XDG_CONFIG_HOME);
    saveEnvironment(yarp::conf::dirs::XDG_CONFIG_DIRS);
    saveEnvironment(yarp::conf::dirs::XDG_CACHE_HOME);
    saveEnvironment(yarp::conf::dirs::XDG_RUNTIME_DIR);
    saveEnvironment(yarp::conf::dirs::APPDATA);
    saveEnvironment(yarp::conf::dirs::LOCALAPPDATA);
    saveEnvironment(yarp::conf::dirs::ALLUSERSPROFILE);
    saveEnvironment(yarp::conf::dirs::YARP_DATA_HOME);
    saveEnvironment(yarp::conf::dirs::YARP_DATA_DIRS);
    saveEnvironment(yarp::conf::dirs::YARP_CONFIG_HOME);
    saveEnvironment(yarp::conf::dirs::YARP_CONFIG_DIRS);
    saveEnvironment(yarp::conf::dirs::YARP_CACHE_HOME);
    saveEnvironment(yarp::conf::dirs::YARP_RUNTIME_DIR);

#if defined(_WIN32)
    yarp::conf::environment::set_string(yarp::conf::dirs::USERNAME, "C:\\Users\\yarptest");
    yarp::conf::environment::set_string(yarp::conf::dirs::USERPROFILE, "C:\\Users\\yarptest");
#else
    yarp::conf::environment::set_string(yarp::conf::dirs::USER, "yarptest");
    yarp::conf::environment::set_string(yarp::conf::dirs::HOME, "/home/yarptest");
#endif


    const std::string slash = std::string{yarp::conf::filesystem::preferred_separator};
    const std::string colon = std::string{yarp::conf::environment::path_separator};

#if defined(_WIN32)
    const std::string yfoo = "C:\\foo\\yarp";
    const std::string ybar = "C:\\bar\\yarp";
#else
    const std::string yfoo = "/foo/yarp";
    const std::string ybar = "/bar/yarp";
#endif


    SECTION("Test yarp::conf::dirs::home()")
    {
#if defined(_WIN32)
        CHECK(yarp::conf::dirs::home() == "C:\\Users\\yarptest");
#else
        CHECK(yarp::conf::dirs::home() == "/home/yarptest");
#endif
    }



    SECTION("Test yarp::conf::dirs::tempdir()")
    {
#if defined(_WIN32)
        // Test with TEMP set
        yarp::conf::environment::set_string(yarp::conf::dirs::TEMP, "C:\\foo");
        CHECK(yarp::conf::dirs::tempdir() == "C:\\foo");
        yarp::conf::environment::unset(yarp::conf::dirs::TEMP);

        // Test default
        CHECK(yarp::conf::dirs::tempdir() == "C:\\Users\\yarptest\\AppData\\Local\\Temp");
#else
        // Test with TMPDIR set
        yarp::conf::environment::set_string(yarp::conf::dirs::TMPDIR, "/foo");
        CHECK(yarp::conf::dirs::tempdir() == "/foo");
        yarp::conf::environment::unset(yarp::conf::dirs::TMPDIR);

        // Test default
        CHECK(yarp::conf::dirs::tempdir() == "/tmp");
#endif
    }



    SECTION("Test yarp::conf::dirs::datahome()")
    {
        // This should be ignored everywhere
        yarp::conf::environment::set_string(yarp::conf::dirs::YARP_DATA_HOME, "__ignored__");

#if defined(_WIN32)
        // Test with APPDATA set
        yarp::conf::environment::set_string(yarp::conf::dirs::APPDATA, "C:\\foo");
        CHECK(yarp::conf::dirs::datahome() == "C:\\foo");
        yarp::conf::environment::unset(yarp::conf::dirs::APPDATA);

        // Test default
        CHECK(yarp::conf::dirs::datahome() == "C:\\Users\\yarptest\\AppData\\Roaming");

#elif defined(__APPLE__)
        // Test default
        CHECK(yarp::conf::dirs::datahome() == "/home/yarptest/Library/Application Support");

#else
        // Test XDG_DATA_HOME set
        yarp::conf::environment::set_string(yarp::conf::dirs::XDG_DATA_HOME, "/foo");
        CHECK(yarp::conf::dirs::datahome() == "/foo");
        yarp::conf::environment::unset(yarp::conf::dirs::XDG_DATA_HOME);

        // Test default
        CHECK(yarp::conf::dirs::datahome() == "/home/yarptest/.local/share");
#endif

        // Restore environment variables
        yarp::conf::environment::unset(yarp::conf::dirs::YARP_DATA_HOME);
    }



    SECTION("test yarp::conf::dirs::datadirs()")
    {
        // This should be ignored everywhere
        yarp::conf::environment::set_string(yarp::conf::dirs::YARP_DATA_DIRS, "__ignored__");

        std::vector<std::string> dirs;

#if defined(_WIN32)
        // Test ALLUSERSPROFILE set
        yarp::conf::environment::set_string(yarp::conf::dirs::ALLUSERSPROFILE, "C:\\foo");
        dirs = yarp::conf::dirs::datadirs();
        CHECK(dirs.size() == 1);
        CHECK(dirs[0] == "C:\\foo");
        yarp::conf::environment::unset(yarp::conf::dirs::ALLUSERSPROFILE);

        // Test default
        dirs = yarp::conf::dirs::datadirs();
        CHECK(dirs.size() == 1);
        CHECK(dirs[0] == "C:\\ProgramData");

#elif defined(__APPLE__)
        // Test default
        dirs = yarp::conf::dirs::datadirs();
        CHECK(dirs.size() == 2);
        CHECK(dirs[0] == "/usr/local/share");
        CHECK(dirs[1] == "/usr/share");
#else
        // Test XDG_DATA_DIRS set
        yarp::conf::environment::set_string(yarp::conf::dirs::XDG_DATA_DIRS, "/foo:/bar");
        dirs = yarp::conf::dirs::datadirs();
        CHECK(dirs.size() == 2);
        CHECK(dirs[0] == "/foo");
        CHECK(dirs[1] == "/bar");
        yarp::conf::environment::unset(yarp::conf::dirs::XDG_DATA_DIRS);

        // Test default
        dirs = yarp::conf::dirs::datadirs();
        CHECK(dirs.size() == 2);
        CHECK(dirs[0] == "/usr/local/share");
        CHECK(dirs[1] == "/usr/share");
#endif

        // Restore environment variables
        yarp::conf::environment::unset(yarp::conf::dirs::YARP_DATA_DIRS);
    }



    SECTION("Test yarp::conf::dirs::confighome()")
    {
        // This should be ignored everywhere
        yarp::conf::environment::set_string(yarp::conf::dirs::YARP_CONFIG_HOME, "__ignored__");

#if defined(_WIN32)
        // Test with APPDATA set
        yarp::conf::environment::set_string(yarp::conf::dirs::APPDATA, "C:\\foo");
        CHECK(yarp::conf::dirs::confighome() == "C:\\foo");
        yarp::conf::environment::unset(yarp::conf::dirs::APPDATA);

        // Test default
        CHECK(yarp::conf::dirs::confighome() == "C:\\Users\\yarptest\\AppData\\Roaming");

#elif defined(__APPLE__)
        // Test default
        CHECK(yarp::conf::dirs::confighome() == "/home/yarptest/Library/Preferences");

#else
        // Test XDG_CONFIG_HOME set
        yarp::conf::environment::set_string(yarp::conf::dirs::XDG_CONFIG_HOME, "/foo");
        CHECK(yarp::conf::dirs::confighome() == "/foo");
        yarp::conf::environment::unset(yarp::conf::dirs::XDG_CONFIG_HOME);

        // Test default
        CHECK(yarp::conf::dirs::confighome() == "/home/yarptest/.config");
#endif

        // Restore environment variables
        yarp::conf::environment::unset(yarp::conf::dirs::YARP_CONFIG_HOME);
    }



    SECTION("test yarp::conf::dirs::configdirs()")
    {
        // This should be ignored everywhere
        yarp::conf::environment::set_string(yarp::conf::dirs::YARP_CONFIG_DIRS, "__ignored__");

        std::vector<std::string> dirs;

#if defined(_WIN32)
        // Test ALLUSERSPROFILE set
        yarp::conf::environment::set_string(yarp::conf::dirs::ALLUSERSPROFILE, "C:\\foo");
        dirs = yarp::conf::dirs::configdirs();
        CHECK(dirs.size() == 1);
        CHECK(dirs[0] == "C:\\foo");
        yarp::conf::environment::unset(yarp::conf::dirs::ALLUSERSPROFILE);

        // Test default
        dirs = yarp::conf::dirs::configdirs();
        CHECK(dirs.size() == 1);
        CHECK(dirs[0] == "C:\\ProgramData");

#elif defined(__APPLE__)
        // Test default
        dirs = yarp::conf::dirs::configdirs();
        CHECK(dirs.size() == 2);
        CHECK(dirs[0] == "/etc");
        CHECK(dirs[1] == "/Library/Preferences");
#else

        // Test XDG_CONFIG_DIRS set
        yarp::conf::environment::set_string(yarp::conf::dirs::XDG_CONFIG_DIRS, "/foo:/bar");
        dirs = yarp::conf::dirs::configdirs();
        CHECK(dirs.size() == 2);
        CHECK(dirs[0] == "/foo");
        CHECK(dirs[1] == "/bar");
        yarp::conf::environment::unset(yarp::conf::dirs::XDG_CONFIG_DIRS);

        // Test default
        dirs = yarp::conf::dirs::configdirs();
        CHECK(dirs.size() == 1);
        CHECK(dirs[0] == "/etc/xdg");
#endif

        // Restore environment variables
        yarp::conf::environment::unset(yarp::conf::dirs::YARP_CONFIG_DIRS);
    }



    SECTION("Test yarp::conf::dirs::cachehome()")
    {
        // This should be ignored everywhere
        yarp::conf::environment::set_string(yarp::conf::dirs::YARP_CACHE_HOME, "__ignored__");

#if defined(_WIN32)
        // Test with LOCALAPPDATA set
        yarp::conf::environment::set_string(yarp::conf::dirs::LOCALAPPDATA, "C:\\foo");
        CHECK(yarp::conf::dirs::cachehome() == "C:\\foo");
        yarp::conf::environment::unset(yarp::conf::dirs::LOCALAPPDATA);

        // Test default
        CHECK(yarp::conf::dirs::cachehome() == "C:\\Users\\yarptest\\AppData\\Local");

#elif defined(__APPLE__)
        // Test default
        CHECK(yarp::conf::dirs::cachehome() == "/home/yarptest/Library/Caches");

#else
        // Test XDG_CACHE_HOME set
        yarp::conf::environment::set_string(yarp::conf::dirs::XDG_CACHE_HOME, "/foo");
        CHECK(yarp::conf::dirs::cachehome() == "/foo");
        yarp::conf::environment::unset(yarp::conf::dirs::XDG_CACHE_HOME);

        // Test default
        CHECK(yarp::conf::dirs::cachehome() == "/home/yarptest/.cache");
#endif

        // Restore environment variables
        yarp::conf::environment::unset(yarp::conf::dirs::YARP_CACHE_HOME);
    }



    SECTION("Test yarp::conf::dirs::runtimedir()")
    {
        // This should be ignored everywhere
        yarp::conf::environment::set_string(yarp::conf::dirs::YARP_RUNTIME_DIR, "__ignored__");

#if defined(_WIN32)
        // Test default
        CHECK(yarp::conf::dirs::runtimedir() == "C:\\Users\\yarptest\\AppData\\Local\\Temp\\runtime");

#elif defined(__APPLE__)
        // Test default
        CHECK(yarp::conf::dirs::runtimedir() == "/tmp/runtime-yarptest");
#else
        // Test XDG_RUNTIME_DIR set
        yarp::conf::environment::set_string(yarp::conf::dirs::XDG_RUNTIME_DIR, "/foo");
        CHECK(yarp::conf::dirs::runtimedir() == "/foo");
        yarp::conf::environment::unset(yarp::conf::dirs::XDG_RUNTIME_DIR);

        // Test default
        CHECK(yarp::conf::dirs::runtimedir() == "/tmp/runtime-yarptest");
#endif

        // Restore environment variables
        yarp::conf::environment::unset(yarp::conf::dirs::YARP_RUNTIME_DIR);
    }



    SECTION("Test yarp::conf::dirs::yarpdatahome()")
    {
#if defined(_WIN32)
        // Test with YARP_DATA_HOME and APPDATA set
        yarp::conf::environment::set_string(yarp::conf::dirs::YARP_DATA_HOME, "C:\\bar");
        yarp::conf::environment::set_string(yarp::conf::dirs::APPDATA, "C:\\foo");
        CHECK(yarp::conf::dirs::yarpdatahome() == "C:\\bar");
        yarp::conf::environment::unset(yarp::conf::dirs::YARP_DATA_HOME);
        yarp::conf::environment::unset(yarp::conf::dirs::APPDATA);

        // Test with YARP_DATA_HOME set
        yarp::conf::environment::set_string(yarp::conf::dirs::YARP_DATA_HOME, "C:\\bar");
        CHECK(yarp::conf::dirs::yarpdatahome() == "C:\\bar");
        yarp::conf::environment::unset(yarp::conf::dirs::YARP_DATA_HOME);

        // Test with APPDATA set
        yarp::conf::environment::set_string(yarp::conf::dirs::APPDATA, "C:\\foo");
        CHECK(yarp::conf::dirs::yarpdatahome() == "C:\\foo\\yarp");
        yarp::conf::environment::unset(yarp::conf::dirs::APPDATA);

        // Test default
        CHECK(yarp::conf::dirs::yarpdatahome() == "C:\\Users\\yarptest\\AppData\\Roaming\\yarp");

#elif defined(__APPLE__)
        // Test with YARP_DATA_HOME set
        yarp::conf::environment::set_string(yarp::conf::dirs::YARP_DATA_HOME, "/bar");
        CHECK(yarp::conf::dirs::yarpdatahome() == "/bar");
        yarp::conf::environment::unset(yarp::conf::dirs::YARP_DATA_HOME);

        // Test default
        CHECK(yarp::conf::dirs::yarpdatahome() == "/home/yarptest/Library/Application Support/yarp");

#else
        // Test with YARP_DATA_HOME and XDG_DATA_HOME set
        yarp::conf::environment::set_string(yarp::conf::dirs::YARP_DATA_HOME, "/bar");
        yarp::conf::environment::set_string(yarp::conf::dirs::XDG_DATA_HOME, "/foo");
        CHECK(yarp::conf::dirs::yarpdatahome() == "/bar");
        yarp::conf::environment::unset(yarp::conf::dirs::YARP_DATA_HOME);
        yarp::conf::environment::unset(yarp::conf::dirs::XDG_DATA_HOME);

        // Test with YARP_DATA_HOME set
        yarp::conf::environment::set_string(yarp::conf::dirs::YARP_DATA_HOME, "/bar");
        CHECK(yarp::conf::dirs::yarpdatahome() == "/bar");
        yarp::conf::environment::unset(yarp::conf::dirs::YARP_DATA_HOME);

        // Test XDG_DATA_HOME set
        yarp::conf::environment::set_string(yarp::conf::dirs::XDG_DATA_HOME, "/foo");
        CHECK(yarp::conf::dirs::yarpdatahome() == "/foo/yarp");
        yarp::conf::environment::unset(yarp::conf::dirs::XDG_DATA_HOME);

        // Test default
        CHECK(yarp::conf::dirs::yarpdatahome() == "/home/yarptest/.local/share/yarp");
#endif
    }



    SECTION("test yarp::conf::dirs::yarpdatadirs()")
    {
        std::vector<std::string> dirs;

#if defined(_WIN32)
        // Test YARP_DATA_DIRS and ALLUSERSPROFILE set
        yarp::conf::environment::set_string(yarp::conf::dirs::YARP_DATA_DIRS, "C:\\baz;C:\\zot");
        yarp::conf::environment::set_string(yarp::conf::dirs::ALLUSERSPROFILE, "C:\\foo");
        dirs = yarp::conf::dirs::yarpdatadirs();
        CHECK(dirs.size() == 2);
        CHECK(dirs[0] == "C:\\baz");
        CHECK(dirs[1] == "C:\\zot");
        yarp::conf::environment::unset(yarp::conf::dirs::YARP_DATA_DIRS);
        yarp::conf::environment::unset(yarp::conf::dirs::ALLUSERSPROFILE);

        // Test YARP_DATA_DIRS set
        yarp::conf::environment::set_string(yarp::conf::dirs::YARP_DATA_DIRS, "C:\\baz;C:\\zot");
        dirs = yarp::conf::dirs::yarpdatadirs();
        CHECK(dirs.size() == 2);
        CHECK(dirs[0] == "C:\\baz");
        CHECK(dirs[1] == "C:\\zot");
        yarp::conf::environment::unset(yarp::conf::dirs::YARP_DATA_DIRS);

        // Test ALLUSERSPROFILE set
        yarp::conf::environment::set_string(yarp::conf::dirs::ALLUSERSPROFILE, "C:\\foo");
        dirs = yarp::conf::dirs::yarpdatadirs();
        CHECK(dirs.size() == 1);
        CHECK(dirs[0] == "C:\\foo\\yarp");
        yarp::conf::environment::unset(yarp::conf::dirs::ALLUSERSPROFILE);

        // Test default
        dirs = yarp::conf::dirs::yarpdatadirs();
        CHECK(dirs.size() == 1);
        CHECK(dirs[0] == "C:\\ProgramData\\yarp");

#elif defined(__APPLE__)
        // Test YARP_DATA_DIRS set
        yarp::conf::environment::set_string(yarp::conf::dirs::YARP_DATA_DIRS, "/baz:/zot");
        dirs = yarp::conf::dirs::yarpdatadirs();
        CHECK(dirs.size() == 2);
        CHECK(dirs[0] == "/baz");
        CHECK(dirs[1] == "/zot");
        yarp::conf::environment::unset(yarp::conf::dirs::YARP_DATA_DIRS);

        // Test default
        dirs = yarp::conf::dirs::yarpdatadirs();
        CHECK(dirs.size() == 2);
        CHECK(dirs[0] == "/usr/local/share/yarp");
        CHECK(dirs[1] == "/usr/share/yarp");
#else
        // Test YARP_DATA_DIRS and XDG_DATA_DIRS set
        yarp::conf::environment::set_string(yarp::conf::dirs::YARP_DATA_DIRS, "/baz:/zot");
        yarp::conf::environment::set_string(yarp::conf::dirs::XDG_DATA_DIRS, "/foo:/bar");
        dirs = yarp::conf::dirs::yarpdatadirs();
        CHECK(dirs.size() == 2);
        CHECK(dirs[0] == "/baz");
        CHECK(dirs[1] == "/zot");
        yarp::conf::environment::unset(yarp::conf::dirs::YARP_DATA_DIRS);
        yarp::conf::environment::unset(yarp::conf::dirs::XDG_DATA_DIRS);

        // Test YARP_DATA_DIRS set
        yarp::conf::environment::set_string(yarp::conf::dirs::YARP_DATA_DIRS, "/baz:/zot");
        dirs = yarp::conf::dirs::yarpdatadirs();
        CHECK(dirs.size() == 2);
        CHECK(dirs[0] == "/baz");
        CHECK(dirs[1] == "/zot");
        yarp::conf::environment::unset(yarp::conf::dirs::YARP_DATA_DIRS);

        // Test XDG_DATA_DIRS set
        yarp::conf::environment::set_string(yarp::conf::dirs::XDG_DATA_DIRS, "/foo:/bar");
        dirs = yarp::conf::dirs::yarpdatadirs();
        CHECK(dirs.size() == 2);
        CHECK(dirs[0] == "/foo/yarp");
        CHECK(dirs[1] == "/bar/yarp");
        yarp::conf::environment::unset(yarp::conf::dirs::XDG_DATA_DIRS);

        // Test default
        dirs = yarp::conf::dirs::yarpdatadirs();
        CHECK(dirs.size() == 2);
        CHECK(dirs[0] == "/usr/local/share/yarp");
        CHECK(dirs[1] == "/usr/share/yarp");
#endif
    }



    SECTION("Test yarp::conf::dirs::yarpconfighome()")
    {
#if defined(_WIN32)
        // Test YARP_CONFIG_HOME and APPDATA set
        yarp::conf::environment::set_string(yarp::conf::dirs::YARP_CONFIG_HOME, "C:\\bar");
        yarp::conf::environment::set_string(yarp::conf::dirs::APPDATA, "C:\\foo");
        CHECK(yarp::conf::dirs::yarpconfighome() == "C:\\bar");
        yarp::conf::environment::unset(yarp::conf::dirs::YARP_CONFIG_HOME);
        yarp::conf::environment::unset(yarp::conf::dirs::APPDATA);

        // Test YARP_CONFIG_HOME set
        yarp::conf::environment::set_string(yarp::conf::dirs::YARP_CONFIG_HOME, "C:\\bar");
        CHECK(yarp::conf::dirs::yarpconfighome() == "C:\\bar");
        yarp::conf::environment::unset(yarp::conf::dirs::YARP_CONFIG_HOME);

        // Test with APPDATA set
        yarp::conf::environment::set_string(yarp::conf::dirs::APPDATA, "C:\\foo");
        CHECK(yarp::conf::dirs::yarpconfighome() == "C:\\foo\\yarp\\config");
        yarp::conf::environment::unset(yarp::conf::dirs::APPDATA);

        // Test default
        CHECK(yarp::conf::dirs::yarpconfighome() == "C:\\Users\\yarptest\\AppData\\Roaming\\yarp\\config");

#elif defined(__APPLE__)
        // Test YARP_CONFIG_HOME set
        yarp::conf::environment::set_string(yarp::conf::dirs::YARP_CONFIG_HOME, "/bar");
        CHECK(yarp::conf::dirs::yarpconfighome() == "/bar");
        yarp::conf::environment::unset(yarp::conf::dirs::YARP_CONFIG_HOME);

        // Test default
        CHECK(yarp::conf::dirs::yarpconfighome() == "/home/yarptest/Library/Preferences/yarp");

#else
        // Test YARP_CONFIG_HOME and XDG_CONFIG_HOME set
        yarp::conf::environment::set_string(yarp::conf::dirs::YARP_CONFIG_HOME, "/bar");
        yarp::conf::environment::set_string(yarp::conf::dirs::XDG_CONFIG_HOME, "/foo");
        CHECK(yarp::conf::dirs::yarpconfighome() == "/bar");
        yarp::conf::environment::unset(yarp::conf::dirs::YARP_CONFIG_HOME);
        yarp::conf::environment::unset(yarp::conf::dirs::XDG_CONFIG_HOME);

        // Test YARP_CONFIG_HOME set
        yarp::conf::environment::set_string(yarp::conf::dirs::YARP_CONFIG_HOME, "/bar");
        CHECK(yarp::conf::dirs::yarpconfighome() == "/bar");
        yarp::conf::environment::unset(yarp::conf::dirs::YARP_CONFIG_HOME);

        // Test XDG_CONFIG_HOME set
        yarp::conf::environment::set_string(yarp::conf::dirs::XDG_CONFIG_HOME, "/foo");
        CHECK(yarp::conf::dirs::yarpconfighome() == "/foo/yarp");
        yarp::conf::environment::unset(yarp::conf::dirs::XDG_CONFIG_HOME);

        // Test default
        CHECK(yarp::conf::dirs::yarpconfighome() == "/home/yarptest/.config/yarp");
#endif
    }



    SECTION("test yarp::conf::dirs::yarpconfigdirs()")
    {
        std::vector<std::string> dirs;

#if defined(_WIN32)
        // Test YARP_CONFIG_DIRS and ALLUSERSPROFILE set
        yarp::conf::environment::set_string(yarp::conf::dirs::YARP_CONFIG_DIRS, "C:\\baz;C:\\zot");
        yarp::conf::environment::set_string(yarp::conf::dirs::ALLUSERSPROFILE, "C:\\foo");
        dirs = yarp::conf::dirs::yarpconfigdirs();
        CHECK(dirs.size() == 2);
        CHECK(dirs[0] == "C:\\baz");
        CHECK(dirs[1] == "C:\\zot");
        yarp::conf::environment::unset(yarp::conf::dirs::YARP_CONFIG_DIRS);
        yarp::conf::environment::unset(yarp::conf::dirs::ALLUSERSPROFILE);

        // Test YARP_CONFIG_DIRS set
        yarp::conf::environment::set_string(yarp::conf::dirs::YARP_CONFIG_DIRS, "C:\\baz;C:\\zot");
        dirs = yarp::conf::dirs::yarpconfigdirs();
        CHECK(dirs.size() == 2);
        CHECK(dirs[0] == "C:\\baz");
        CHECK(dirs[1] == "C:\\zot");
        yarp::conf::environment::unset(yarp::conf::dirs::YARP_CONFIG_DIRS);

        // Test ALLUSERSPROFILE set
        yarp::conf::environment::set_string(yarp::conf::dirs::ALLUSERSPROFILE, "C:\\foo");
        dirs = yarp::conf::dirs::yarpconfigdirs();
        CHECK(dirs.size() == 1);
        CHECK(dirs[0] == "C:\\foo\\yarp\\config");
        yarp::conf::environment::unset(yarp::conf::dirs::ALLUSERSPROFILE);

        // Test default
        dirs = yarp::conf::dirs::yarpconfigdirs();
        CHECK(dirs.size() == 1);
        CHECK(dirs[0] == "C:\\ProgramData\\yarp\\config");

#elif defined(__APPLE__)
        // Test YARP_CONFIG_DIRS set
        yarp::conf::environment::set_string(yarp::conf::dirs::YARP_CONFIG_DIRS, "/baz:/zot");
        dirs = yarp::conf::dirs::yarpconfigdirs();
        CHECK(dirs.size() == 2);
        CHECK(dirs[0] == "/baz");
        CHECK(dirs[1] == "/zot");
        yarp::conf::environment::unset(yarp::conf::dirs::YARP_CONFIG_DIRS);

        // Test default
        dirs = yarp::conf::dirs::yarpconfigdirs();
        CHECK(dirs.size() == 2);
        CHECK(dirs[0] == "/etc/yarp");
        CHECK(dirs[1] == "/Library/Preferences/yarp");

#else
        // Test YARP_CONFIG_DIRS and XDG_CONFIG_DIRS set
        yarp::conf::environment::set_string(yarp::conf::dirs::YARP_CONFIG_DIRS, "/baz:/zot");
        yarp::conf::environment::set_string(yarp::conf::dirs::XDG_CONFIG_DIRS, "/foo:/bar");
        dirs = yarp::conf::dirs::yarpconfigdirs();
        CHECK(dirs.size() == 2);
        CHECK(dirs[0] == "/baz");
        CHECK(dirs[1] == "/zot");
        yarp::conf::environment::unset(yarp::conf::dirs::YARP_CONFIG_DIRS);
        yarp::conf::environment::unset(yarp::conf::dirs::XDG_CONFIG_DIRS);

        // Test YARP_CONFIG_DIRS set
        yarp::conf::environment::set_string(yarp::conf::dirs::YARP_CONFIG_DIRS, "/baz:/zot");
        dirs = yarp::conf::dirs::yarpconfigdirs();
        CHECK(dirs.size() == 2);
        CHECK(dirs[0] == "/baz");
        CHECK(dirs[1] == "/zot");
        yarp::conf::environment::unset(yarp::conf::dirs::YARP_CONFIG_DIRS);

        // Test XDG_CONFIG_DIRS set
        yarp::conf::environment::set_string(yarp::conf::dirs::XDG_CONFIG_DIRS, "/foo:/bar");
        dirs = yarp::conf::dirs::yarpconfigdirs();
        CHECK(dirs.size() == 2);
        CHECK(dirs[0] == "/foo/yarp");
        CHECK(dirs[1] == "/bar/yarp");
        yarp::conf::environment::unset(yarp::conf::dirs::XDG_CONFIG_DIRS);

        // Test default
        dirs = yarp::conf::dirs::yarpconfigdirs();
        CHECK(dirs.size() == 1);
        CHECK(dirs[0] == "/etc/xdg/yarp");
#endif
    }



    SECTION("Test yarp::conf::dirs::yarpcachehome()")
    {
#if defined(_WIN32)
        // Test with YARP_CACHE_HOME and LOCALAPPDATA set
        yarp::conf::environment::set_string(yarp::conf::dirs::YARP_CACHE_HOME, "C:\\bar");
        yarp::conf::environment::set_string(yarp::conf::dirs::LOCALAPPDATA, "C:\\foo");
        CHECK(yarp::conf::dirs::yarpcachehome() == "C:\\bar");
        yarp::conf::environment::unset(yarp::conf::dirs::YARP_CACHE_HOME);
        yarp::conf::environment::unset(yarp::conf::dirs::LOCALAPPDATA);

        // Test with YARP_CACHE_HOME set
        yarp::conf::environment::set_string(yarp::conf::dirs::YARP_CACHE_HOME, "C:\\bar");
        CHECK(yarp::conf::dirs::yarpcachehome() == "C:\\bar");
        yarp::conf::environment::unset(yarp::conf::dirs::YARP_CACHE_HOME);

        // Test with LOCALAPPDATA set
        yarp::conf::environment::set_string(yarp::conf::dirs::LOCALAPPDATA, "C:\\foo");
        CHECK(yarp::conf::dirs::yarpcachehome() == "C:\\foo\\yarp");
        yarp::conf::environment::unset(yarp::conf::dirs::LOCALAPPDATA);

        // Test default
        CHECK(yarp::conf::dirs::yarpcachehome() == "C:\\Users\\yarptest\\AppData\\Local\\yarp");

#elif defined(__APPLE__)
        // Test with YARP_CACHE_HOME set
        yarp::conf::environment::set_string(yarp::conf::dirs::YARP_CACHE_HOME, "/bar");
        CHECK(yarp::conf::dirs::yarpcachehome() == "/bar");
        yarp::conf::environment::unset(yarp::conf::dirs::YARP_CACHE_HOME);

        // Test default
        CHECK(yarp::conf::dirs::yarpcachehome() == "/home/yarptest/Library/Caches/yarp");

#else
        // Test XDG_CACHE_HOME set
        yarp::conf::environment::set_string(yarp::conf::dirs::XDG_CACHE_HOME, "/foo");
        CHECK(yarp::conf::dirs::yarpcachehome() == "/foo/yarp");
        yarp::conf::environment::unset(yarp::conf::dirs::XDG_CACHE_HOME);

        // Test default
        CHECK(yarp::conf::dirs::yarpcachehome() == "/home/yarptest/.cache/yarp");
#endif
    }



    SECTION("Test yarp::conf::dirs::yarpruntimedir()")
    {
#if defined(_WIN32)
        // Test YARP_RUNTIME_DIR set
        yarp::conf::environment::set_string(yarp::conf::dirs::YARP_RUNTIME_DIR, "C:\\bar");
        CHECK(yarp::conf::dirs::yarpruntimedir() == "C:\\bar");
        yarp::conf::environment::unset(yarp::conf::dirs::YARP_RUNTIME_DIR);

        // Test default
        CHECK(yarp::conf::dirs::yarpruntimedir() == "C:\\Users\\yarptest\\AppData\\Local\\Temp\\runtime\\yarp");

#elif defined(__APPLE__)
        // Test YARP_RUNTIME_DIR set
        yarp::conf::environment::set_string(yarp::conf::dirs::YARP_RUNTIME_DIR, "/bar");
        CHECK(yarp::conf::dirs::yarpruntimedir() == "/bar");
        yarp::conf::environment::unset(yarp::conf::dirs::YARP_RUNTIME_DIR);

        // Test default
        CHECK(yarp::conf::dirs::yarpruntimedir() == "/tmp/runtime-yarptest/yarp");
#else
        // Test YARP_RUNTIME_DIR and XDG_RUNTIME_DIR set
        yarp::conf::environment::set_string(yarp::conf::dirs::YARP_RUNTIME_DIR, "/bar");
        yarp::conf::environment::set_string(yarp::conf::dirs::XDG_RUNTIME_DIR, "/foo");
        CHECK(yarp::conf::dirs::yarpruntimedir() == "/bar");
        yarp::conf::environment::unset(yarp::conf::dirs::YARP_RUNTIME_DIR);
        yarp::conf::environment::unset(yarp::conf::dirs::XDG_RUNTIME_DIR);

        // Test YARP_RUNTIME_DIR set
        yarp::conf::environment::set_string(yarp::conf::dirs::YARP_RUNTIME_DIR, "/bar");
        CHECK(yarp::conf::dirs::yarpruntimedir() == "/bar");
        yarp::conf::environment::unset(yarp::conf::dirs::YARP_RUNTIME_DIR);

        // Test XDG_RUNTIME_DIR set
        yarp::conf::environment::set_string(yarp::conf::dirs::XDG_RUNTIME_DIR, "/foo");
        CHECK(yarp::conf::dirs::yarpruntimedir() == "/foo/yarp");
        yarp::conf::environment::unset(yarp::conf::dirs::XDG_RUNTIME_DIR);

        // Test default
        CHECK(yarp::conf::dirs::yarpruntimedir() == "/tmp/runtime-yarptest/yarp");
#endif
    }

    restoreEnvironment();
}
