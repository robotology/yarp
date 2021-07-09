/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */


#ifndef YARP_CONF_DIRS_H
#define YARP_CONF_DIRS_H

#include <yarp/conf/filesystem.h>
#include <yarp/conf/environment.h>

#include <string>
#include <vector>

namespace yarp {
namespace conf {
namespace dirs {

#ifndef DOXYGEN_SHOULD_SKIP_THIS
#ifndef SWIG

// Constants defining environment variables names
static constexpr const char YARP_DATA_HOME[]{"YARP_DATA_HOME"};
static constexpr const char YARP_DATA_DIRS[]{"YARP_DATA_DIRS"};
static constexpr const char YARP_CONFIG_HOME[]{"YARP_CONFIG_HOME"};
static constexpr const char YARP_CONFIG_DIRS[]{"YARP_CONFIG_DIRS"};
static constexpr const char YARP_CACHE_HOME[]{"YARP_CACHE_HOME"};
static constexpr const char YARP_RUNTIME_DIR[]{"YARP_RUNTIME_DIR"};

static constexpr const char USER[]{"USER"};
static constexpr const char USERNAME[]{"USERNAME"};
static constexpr const char HOME[]{"HOME"};
static constexpr const char USERPROFILE[]{"USERPROFILE"};
static constexpr const char TMP[]{"TMP"};
static constexpr const char TEMP[]{"TEMP"};
static constexpr const char TMPDIR[]{"TMPDIR"};
static constexpr const char XDG_DATA_HOME[]{"XDG_DATA_HOME"};
static constexpr const char XDG_DATA_DIRS[]{"XDG_DATA_DIRS"};
static constexpr const char XDG_CONFIG_HOME[]{"XDG_CONFIG_HOME"};
static constexpr const char XDG_CONFIG_DIRS[]{"XDG_CONFIG_DIRS"};
static constexpr const char XDG_CACHE_HOME[]{"XDG_CACHE_HOME"};
static constexpr const char XDG_RUNTIME_DIR[]{"XDG_RUNTIME_DIR"};
static constexpr const char APPDATA[]{"APPDATA"};
static constexpr const char LOCALAPPDATA[]{"LOCALAPPDATA"};
static constexpr const char ALLUSERSPROFILE[]{"ALLUSERSPROFILE"};

// Paths and suffixes
static constexpr const char XDG_DATA_HOME_SUFFIX[]{"/.local/share"};
static constexpr const char XDG_CONFIG_HOME_SUFFIX[]{"/.config"};
static constexpr const char XDG_CACHE_HOME_SUFFIX[]{"/.cache"};
static constexpr const char XDG_DATA_DIRS_DEFAULT[]{"/usr/local/share:/usr/share"};
static constexpr const char XDG_CONFIG_DIRS_DEFAULT[]{"/etc/xdg"};

static constexpr const char UNIX_TMP_DIR_DEFAULT[]{"/tmp"};
static constexpr const char WIN_APPDATA_SUFFIX[]{"\\AppData\\Roaming"};
static constexpr const char WIN_LOCALAPPDATA_SUFFIX[]{"\\AppData\\Local"};
static constexpr const char WIN_APPDATA_LOCAL_TEMP_SUFFIX[]{"\\AppData\\Local\\Temp"};
static constexpr const char WIN_ALLUSERSPROFILE_DEFAULT[]{"C:\\ProgramData"};
static constexpr const char MACOS_DATAHOME_SUFFIX[]{"/Library/Application Support"};
static constexpr const char MACOS_CONFIGHOME_SUFFIX[]{"/Library/Preferences"};
static constexpr const char MACOS_DATA_DIRS_DEFAULT[]{"/usr/local/share:/usr/share"};
static constexpr const char MACOS_CONFIG_DIRS_DEFAULT[]{"/etc:/Library/Preferences"};
static constexpr const char MACOS_CACHEHOME_SUFFIX[]{"/Library/Caches"};

// FIXME C++17 Use string_view?
static constexpr const char YARP_SUFFIX[]{ yarp::conf::filesystem::preferred_separator, 'y', 'a', 'r', 'p', '\0' }; // "/yarp" or "\\yarp"
static constexpr const char YARP_CONFIG_SUFFIX[]{ yarp::conf::filesystem::preferred_separator, 'y', 'a', 'r', 'p', yarp::conf::filesystem::preferred_separator, 'c', 'o', 'n', 'f', 'i', 'g', '\0' }; // "yarp/config" or "\\yarp\\config"
static constexpr const char RUNTIME_SUFFIX[]{ yarp::conf::filesystem::preferred_separator, 'r', 'u', 'n', 't', 'i', 'm', 'e', '\0' };  // "/runtime" or "\\runtime"
static constexpr const char RUNTIME_YARP_SUFFIX[]{ yarp::conf::filesystem::preferred_separator, 'r', 'u', 'n', 't', 'i', 'm', 'e', yarp::conf::filesystem::preferred_separator, 'y', 'a', 'r', 'p', '\0' };  // "/runtime/yarp" or "\\runtime\\yarp"

#endif // SWIG
#endif // DOXYGEN_SHOULD_SKIP_THIS

/** @{ */
// Basic paths

/**
 * @brief Returns the home directory for current user.
 *
 *  - Windows: `USERPROFILE` environment variable
 *  - Others: `HOME` environment variable
 *
 * @since YARP 3.5
 */
inline std::string home()
{
#if defined(_WIN32)
    return yarp::conf::environment::get_string(USERPROFILE);
#else
    return yarp::conf::environment::get_string(HOME);
#endif
}

/**
 * @brief Returns the directory for temporary files.
 *
 *  - Windows: `TEMP` or `TMP` environment variables (default: `[HOME]\AppData\Local\Temp`)
 *  - Others: `TMPDIR` environment variables (default: `/tmp`)
 *
 * @since YARP 3.5
 */
inline std::string tempdir()
{
#if defined(_WIN32)
    return yarp::conf::environment::get_string(TEMP, yarp::conf::dirs::home() + WIN_APPDATA_LOCAL_TEMP_SUFFIX);
#else
    return yarp::conf::environment::get_string(TMPDIR, UNIX_TMP_DIR_DEFAULT);
#endif
}


/** @} */
/** @{ */
// XDG Base Directory specifications (and equivalents for windows and macos)

/**
 * @brief Returns the directory where user-specific data files should be
 *        written.
 *
 *  - Windows: `APPDATA` environment variable (default: `[HOME]\AppData\Roaming`)
 *  - macOS: `[HOME]/Library/Application Support`
 *  - Others: `XDG_DATA_HOME` environment variable (default: `[HOME]/.local/share`)
 *
 * @since YARP 3.5
 */
inline std::string datahome()
{
#if defined(_WIN32)
    return yarp::conf::environment::get_string(APPDATA, yarp::conf::dirs::home() + WIN_APPDATA_SUFFIX);
#elif defined(__APPLE__)
    return yarp::conf::dirs::home() + MACOS_DATAHOME_SUFFIX;
#else
    return yarp::conf::environment::get_string(XDG_DATA_HOME, yarp::conf::dirs::home() + XDG_DATA_HOME_SUFFIX);
#endif
}

/**
 * @brief Returns the directories where data files should be searched.
 *
 *  - Windows: `ALLUSERSPROFILE` environment variable (default: `C:\\ProgramData`)
 *  - macOS: `/usr/local/share:/usr/share`
 *  - Others: `XDG_DATA_DIRS` environment variable (default: `/usr/local/share:/usr/share`)
 *
 * @since YARP 3.5
 */
inline std::vector<std::string> datadirs()
{
#if defined(_WIN32)
    return yarp::conf::environment::get_path(ALLUSERSPROFILE, WIN_ALLUSERSPROFILE_DEFAULT);
#elif defined(__APPLE__)
    return yarp::conf::environment::split_path(MACOS_DATA_DIRS_DEFAULT);
#else
    return yarp::conf::environment::get_path(XDG_DATA_DIRS, XDG_DATA_DIRS_DEFAULT);
#endif
}

/**
 * @brief Returns the directory where user-specific configuration files should
 *        be written.
 *
 *  - Windows: `APPDATA` environment variable (default: `[HOME]\AppData\Roaming`)
 *  - macOS: `[HOME]/Library/Preferences`
 *  - Others: `XDG_CONFIG_HOME` environment variable (default: `[HOME]/.config`)
 *
 * @since YARP 3.5
 */
inline std::string confighome()
{
#if defined(_WIN32)
    return yarp::conf::environment::get_string(APPDATA, yarp::conf::dirs::home() + WIN_APPDATA_SUFFIX);
#elif defined(__APPLE__)
    return yarp::conf::dirs::home() + MACOS_CONFIGHOME_SUFFIX;
#else
    return yarp::conf::environment::get_string(XDG_CONFIG_HOME, yarp::conf::dirs::home() + XDG_CONFIG_HOME_SUFFIX);
#endif
}

/**
 * @brief Returns the directories where configuration files should be searched.
 *
 *  - Windows: `ALLUSERSPROFILE` environment variable (default: `C:\\ProgramData`)
 *  - macOS: `/etc:/Library/Preferences`
 *  - Others: `XDG_CONFIG_DIRS` environment variable (default: `/etc/xdg`)
 *
 * @since YARP 3.5
 */
inline std::vector<std::string> configdirs()
{
#if defined(_WIN32)
    return yarp::conf::environment::get_path(ALLUSERSPROFILE, WIN_ALLUSERSPROFILE_DEFAULT);
#elif defined(__APPLE__)
    return yarp::conf::environment::split_path(MACOS_CONFIG_DIRS_DEFAULT);
#else
    return yarp::conf::environment::get_path(XDG_CONFIG_DIRS, XDG_CONFIG_DIRS_DEFAULT);
#endif
}

/**
 * @brief Returns the directory where user-specific non-essential (cached) data
 *        should be written.
 *
 *  - Windows: `LOCALAPPDATA` environment variable (default: `[HOME]\AppData\Local`)
 *  - macOS: `[HOME]/Library/Caches`
 *  - Others: `XDG_CACHE_HOME` environment variable (default: `[HOME]/.cache`)
 *
 * @since YARP 3.5
 */
inline std::string cachehome()
{
#if defined(_WIN32)
    return yarp::conf::environment::get_string(LOCALAPPDATA, yarp::conf::dirs::home() + WIN_LOCALAPPDATA_SUFFIX);
#elif defined(__APPLE__)
    return yarp::conf::dirs::home() + MACOS_CACHEHOME_SUFFIX;
#else
    return yarp::conf::environment::get_string(XDG_CACHE_HOME, yarp::conf::dirs::home() + XDG_CACHE_HOME_SUFFIX);
#endif
}

/**
 * @brief Returns the directory where user-specific runtime files and other file
 *        objects should be placed.
 *
 *  - Windows: [TEMP]\runtime
 *  - macOS: [TEMP]/runtime-[USER]
 *  - Others: `XDG_RUNTIME_DIR` environment variable (default: `[TEMP]/runtime-[USER]`)
 *
 * @since YARP 3.5
 */
inline std::string runtimedir()
{
#if defined(_WIN32)
    return yarp::conf::dirs::tempdir() + RUNTIME_SUFFIX;
#elif defined(__APPLE__)
    return yarp::conf::dirs::tempdir() + RUNTIME_SUFFIX + '-' + yarp::conf::environment::get_string(USER);
#else
    return yarp::conf::environment::get_string(XDG_RUNTIME_DIR, yarp::conf::dirs::tempdir() + RUNTIME_SUFFIX + '-' + yarp::conf::environment::get_string(USER));
#endif
}


/** @} */
/** @{ */
// YARP Base Directory specifications

/**
 * @brief Returns the directory where user-specific YARP data files should be
 *        written.
 *
 * Uses `YARP_DATA_HOME` environment variable, if defined.
 * Otherwise uses the "yarp" folder inside the folder returned by
 * yarp::conf::dirs::datahome()
 *
 * @since YARP 3.5
 */
inline std::string yarpdatahome()
{
#if defined(_WIN32)
    return yarp::conf::environment::get_string(YARP_DATA_HOME, APPDATA, yarp::conf::dirs::home() + WIN_APPDATA_SUFFIX, YARP_SUFFIX);
#elif defined(__APPLE__)
    return yarp::conf::environment::get_string(YARP_DATA_HOME, yarp::conf::dirs::home() + MACOS_DATAHOME_SUFFIX + YARP_SUFFIX);
#else
    return yarp::conf::environment::get_string(YARP_DATA_HOME, XDG_DATA_HOME, yarp::conf::dirs::home() + XDG_DATA_HOME_SUFFIX, YARP_SUFFIX);
#endif
}

/**
 * @brief Returns the directories where YARP data files should be searched.
 *
 * Uses `YARP_DATA_DIRS` environment variable, if defined.
 * Otherwise uses the "yarp" folder inside each folder returned by
 * yarp::conf::dirs::datadirs()
 *
 * @since YARP 3.5
 */
inline std::vector<std::string> yarpdatadirs()
{
#if defined(_WIN32)
    return yarp::conf::environment::get_path(YARP_DATA_DIRS, ALLUSERSPROFILE, WIN_ALLUSERSPROFILE_DEFAULT, YARP_SUFFIX);
#elif defined(__APPLE__)
    return yarp::conf::environment::get_path(YARP_DATA_DIRS, "", MACOS_DATA_DIRS_DEFAULT, YARP_SUFFIX);
#else
    return yarp::conf::environment::get_path(YARP_DATA_DIRS, XDG_DATA_DIRS, XDG_DATA_DIRS_DEFAULT, YARP_SUFFIX);
#endif
}

/**
 * @brief Returns the directory where user-specific YARP configuration files
 *        should be written.
 *
 * Uses `YARP_CONFIG_HOME` environment variable, if defined.
 * Otherwise uses the `yarp` folder (`yarp\config` on Windows) inside the folder
 * returned by yarp::conf::dirs::confighome()
 *
 * @since YARP 3.5
 */
inline std::string yarpconfighome()
{
#if defined(_WIN32)
    return yarp::conf::environment::get_string(YARP_CONFIG_HOME, APPDATA, yarp::conf::dirs::home() + WIN_APPDATA_SUFFIX, YARP_CONFIG_SUFFIX);
#elif defined(__APPLE__)
    return yarp::conf::environment::get_string(YARP_CONFIG_HOME, yarp::conf::dirs::home() + MACOS_CONFIGHOME_SUFFIX + YARP_SUFFIX);
#else
    return yarp::conf::environment::get_string(YARP_CONFIG_HOME, XDG_CONFIG_HOME, yarp::conf::dirs::home() + XDG_CONFIG_HOME_SUFFIX, YARP_SUFFIX);
#endif
}

/**
 * @brief Returns the directories where YARP configuration files should be
 *        searched.
 *
 * Uses `YARP_CONFIG_DIRS` environment variable, if defined.
 * Otherwise uses the `yarp` folder (`yarp\config` on Windows) inside each
 * folder returned by yarp::conf::dirs::configdirs()
 *
 * @warning On linux, if `XDG_CONFIG_DIRS` is not defined, the folder
 *          `/etc/yarp` is used instead of `/etc/xdg/yarp`. This might change in
 *          the future.
 *
 * @since YARP 3.5
 */
inline std::vector<std::string> yarpconfigdirs()
{
#if defined(_WIN32)
    return yarp::conf::environment::get_path(YARP_CONFIG_DIRS, ALLUSERSPROFILE, WIN_ALLUSERSPROFILE_DEFAULT, YARP_CONFIG_SUFFIX);
#elif defined(__APPLE__)
    return yarp::conf::environment::get_path(YARP_CONFIG_DIRS, "", MACOS_CONFIG_DIRS_DEFAULT, YARP_SUFFIX);
#else
    return yarp::conf::environment::get_path(YARP_CONFIG_DIRS, XDG_CONFIG_DIRS, XDG_CONFIG_DIRS_DEFAULT, YARP_SUFFIX);
#endif
}

/**
 * @brief Returns the directory where user-specific non-essential (cached) YARP
 *        data should be written.
 *
 * Uses `YARP_CACHE_HOME` environment variable, if defined.
 * Otherwise uses the `yarp` folder inside the folder returned by
 * yarp::conf::dirs::cachehome()
 *
 * @since YARP 3.5
 */
inline std::string yarpcachehome()
{
#if defined(_WIN32)
    return yarp::conf::environment::get_string(YARP_CACHE_HOME, LOCALAPPDATA, yarp::conf::dirs::home() + WIN_LOCALAPPDATA_SUFFIX, YARP_SUFFIX);
#elif defined(__APPLE__)
    return yarp::conf::environment::get_string(YARP_CACHE_HOME, yarp::conf::dirs::home() + MACOS_CACHEHOME_SUFFIX + YARP_SUFFIX);
#else
    return yarp::conf::environment::get_string(YARP_CACHE_HOME, XDG_CACHE_HOME, yarp::conf::dirs::home() + XDG_CACHE_HOME_SUFFIX, YARP_SUFFIX);
#endif
}

/**
 * @brief Returns the directory where user-specific runtime YARP files and other
 *        YARP file objects should be placed.
 *
 * Uses `YARP_RUNTIME_DIR` environment variable, if defined.
 * Otherwise uses the `yarp` folder inside the folder returned by
 * yarp::conf::dirs::runtimedir()
 *
 * @since YARP 3.5
 */
inline std::string yarpruntimedir()
{
#if defined(_WIN32)
    return yarp::conf::environment::get_string(YARP_RUNTIME_DIR, yarp::conf::dirs::tempdir() + RUNTIME_YARP_SUFFIX);
#elif defined(__APPLE__)
    return yarp::conf::environment::get_string(YARP_RUNTIME_DIR, yarp::conf::dirs::tempdir() + RUNTIME_SUFFIX + '-' + yarp::conf::environment::get_string(USER) + YARP_SUFFIX);
#else
    return yarp::conf::environment::get_string(YARP_RUNTIME_DIR, XDG_RUNTIME_DIR, yarp::conf::dirs::tempdir() + RUNTIME_SUFFIX + '-' + yarp::conf::environment::get_string(USER), YARP_SUFFIX);
#endif
}
/** @} */

} // namespace dirs
} // namespace conf
} // namespace yarp


#endif // YARP_CONF_DIRS_H
