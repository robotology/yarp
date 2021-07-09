/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */


#ifndef YARP_CONF_ENVIRONMENT_H
#define YARP_CONF_ENVIRONMENT_H

#include <yarp/conf/api.h>
#include <yarp/conf/system.h>
#include <yarp/conf/numeric.h>
#include <yarp/conf/string.h>

#include <cstdlib>
#include <numeric>
#include <regex>
#include <string>
#include <type_traits>
#include <vector>

namespace yarp {
namespace conf {
namespace environment {

#if defined(_WIN32)
static constexpr char path_separator = ';';
#else
static constexpr char path_separator = ':';
#endif


/**
 * Utility to split a string containing a path separated by the
 * @c path_separator, which depends on the system, into a vector of strings.
 *
 * @param s the string to split
 * @return a vector of strings each containing one of the element of the path
 */
template <typename ContainerT = std::vector<std::string>>
inline ContainerT split_path(const typename ContainerT::value_type& s)
{
    return yarp::conf::string::split<ContainerT>(s, typename ContainerT::value_type::value_type{yarp::conf::environment::path_separator});
}

/**
 * Utility to join a vector of strings into a single string separated by the
 * proper @c path_separator, which depends on the system.
 *
 * @param s the vector of strings to join
 * @return a strings containing the generated path
 */
template <typename ContainerT = std::vector<std::string>>
inline typename ContainerT::value_type join_path(const ContainerT& v)
{
    return yarp::conf::string::join<ContainerT>(v.begin(), v.end(), typename ContainerT::value_type::value_type{yarp::conf::environment::path_separator});
}

/**
 * Read a string from an environment variable.
 *
 * @param key the environment variable to read
 * @param found an optional variable to set to true iff variable is found
 * @return the value of the environment variable, or "" if not found
 *
 * @since YARP 3.5
 */
inline std::string get_string(const std::string& key, bool* found = nullptr)
{
    const char* result = std::getenv(key.c_str());
    if (found != nullptr) {
        *found = (result != nullptr);
    }
    if (result == nullptr) {
        return {};
    }
    return std::string(result);
}

/**
 * Read a string from an environment variable with a default value.
 *
 * @param key the environment variable to read
 * @param defaultValue the default value to return if the environment variable is not found
 * @return the value of the environment variable, or \p defaultValue if not found
 *
 * @since YARP 3.5
 */
inline std::string get_string(const std::string& key, const std::string& defaultValue)
{
    const char* result = std::getenv(key.c_str());
    return result ? std::string{result} : defaultValue;
}

/**
 * Read a string from an environment variable with with a fallback.
 *
 * @param key the environment variable to read
 * @param defaultValue the default value to return if the environment variable is not found
 * @return the value of the environment variable, or \p defaultValue if not found
 *
 * @since YARP 3.5
 */
inline std::string get_string(const std::string& key, const std::string& altKey, const std::string& altDefaultValue, const std::string& altAppend = {})
{
    bool found;
    auto ret = get_string(key, &found);
    return (found ? ret : get_string(altKey, altDefaultValue) + altAppend);
}

/**
 * Read a path from an environment variable.
 *
 * @param key the environment variable to read
 * @param found an optional variable to set to true iff variable is found
 * @return the value of the environment variable, or empty if not found
 *
 * @since YARP 3.5
 */
inline std::vector<std::string> get_path(const std::string& key, bool* found = nullptr)
{
    return split_path(get_string(key, found));
}

/**
 * Read a path from an environment variable with a default value.
 *
 * @param key the environment variable to read
 * @param defaultValue the default value to return if the environment variable is not found
 * @return the value of the environment variable, or \p defaultValue if not found
 *
 * @since YARP 3.5
 */
inline std::vector<std::string> get_path(const std::string& key, const std::string& defaultValue)
{
    return split_path(get_string(key, defaultValue));
}

/**
 * Read a path from an environment variable with a fallback.
 *
 * @param key the environment variable to read
 * @param altKey the alternative variable to read if \p key is not defined
 * @param altDefaultValue the default for the alternative value to use if environment variable is not found
 * @param altAppend a string to append to each directory in the alternative variable (or its default) if \p key is not defined
 * @return the value of the environment variable, or \p defaultValue if not found
 *
 * @since YARP 3.5
 */
inline std::vector<std::string> get_path(const std::string& key, const std::string& altKey, const std::string& altDefaultValue, const std::string& altAppend = {})
{
    bool found;
    std::vector<std::string> path = get_path(key, &found);
    if (!found) {
        path = get_path(altKey, altDefaultValue);
        if (!altAppend.empty()) {
            for (auto& dir : path) {
                dir.append(altAppend);
            }
        }
    }
    return path;
}

/**
 * Read a bool value from an environment variable.
 *
 * @param key the environment variable to read
 * @param defaultValue the default value to return if the environment variable is not found
 * @return the value of the environment variable, or \p defaultValue if not found
 *
 * @since YARP 3.5
 */
inline bool get_bool(const std::string& key, bool defaultValue = false)
{
    const char *strvalue = std::getenv(key.c_str());
    if(!strvalue) {
        return defaultValue;
    }
    return yarp::conf::numeric::from_string<bool>(strvalue, defaultValue);
}

/**
 * Read an numeric value from an environment variable.
 *
 * @param key the environment variable to read
 * @param defaultValue the default value to return if the environment variable is not found
 * @return the value of the environment variable, or \p defaultValue if not found
 *
 * @since YARP 3.5
 */
template <typename T>
inline T get_numeric(const std::string& key, T defaultValue = static_cast<T>(0))
{
    const char *strvalue = std::getenv(key.c_str());
    if (!strvalue) {
        return defaultValue;
    }
    return yarp::conf::numeric::from_string<T>(strvalue, defaultValue);
}

/**
 * Set a string to an environment variable.
 *
 * @param key the environment variable to set or change
 * @param value the target value
 * @return true if the environment variable was set, false on error.
 *
 * @since YARP 3.5
 */
inline bool set_string(const std::string& key, const std::string& value)
{
#if defined(_MSC_VER)
    auto ret = _putenv_s(key.c_str(), value.c_str());
#else
    auto ret = ::setenv(key.c_str(), value.c_str(), 1);
#endif
    return (ret == 0);
}

/**
 * Set a path to an environment variable.
 *
 * @param key the environment variable to set or change
 * @param value the target value
 * @return true if the environment variable was set, false on error.
 *
 * @since YARP 3.5
 */
inline bool set_path(const std::string& key, const std::vector<std::string>& value)
{
    return set_string(key, join_path(value));
}

/**
 * Set a bool to an environment variable (as 1 or 0).
 *
 * @param key the environment variable to set or change
 * @param value the target value
 * @return true if the environment variable was set, false on error.
 *
 * @since YARP 3.5
 */
inline bool set_bool(const std::string& key, bool value)
{
    // Boolean environment variables are usually set as "1" or "0", and not
    // as "true" or "false", therefore `to_string` is not used here.
    return set_string(key, value ? "1" : "0");
}

/**
 * Set a numeric value to an environment variable.
 *
 * @param key the environment variable to set or change
 * @param value the target value
 * @return true if the environment variable was set, false on error.
 *
 * @since YARP 3.5
 */
template <typename T>
inline bool set_numeric(const std::string& key, bool value)
{
    return set_string(key, yarp::conf::numeric::to_string(value));
}

/**
 * Remove an environment variable.
 *
 * @param key the environment variable to remove
 *
 * @return true if the operation was successful, false on error.
 * @since YARP 3.5
 */
inline bool unset(const std::string& key)
{
#if defined(_MSC_VER)
    auto ret = _putenv_s(key.c_str(), "");
#else
    auto ret = ::unsetenv(key.c_str());
#endif
    return (ret == 0);
}

/**
 * Check if an environment variable is set.
 *
 * @param key the environment variable to check
 * @return true if the variable is set, false otherwise
 */
inline bool is_set(const std::string& key)
{
    return std::getenv(key.c_str()) ? true : false;
}


#ifndef YARP_NO_DEPRECATED // Since YARP 3.5
/**
 * Read a string from an environment variable.
 *
 * @param key the environment variable to read
 * @param found an optional variable to set to true iff variable is found
 * @return the value of the environment variable, or "" if not found
 *
 * @since YARP 3.4
 * @deprecated since YARP 3.5. Use yarp::conf::environment::get_string() instead
 */
YARP_DEPRECATED_MSG("Use yarp::conf::environment::get_string() instead")
inline std::string
getEnvironment(const char* key, bool* found = nullptr)
{
    return get_string(key, found);
}

/**
 * Set or change an environment variable.
 *
 * @param key the environment variable to set or change
 * @param value the target value
 *
 * @since YARP 3.4
 * @deprecated since YARP 3.5. Use yarp::conf::environment::set_string() instead
 */
YARP_DEPRECATED_MSG("Use yarp::conf::environment::set_string() instead")
inline void setEnvironment(const std::string& key, const std::string& value)
{
    set_string(key, value);
}

/**
 * Remove an environment variable.
 *
 * @param key the environment variable to remove
 *
 * @since YARP 3.4
 * @deprecated since YARP 3.5. Use yarp::conf::environment::unset() instead
 */
YARP_DEPRECATED_MSG("Use yarp::conf::environment::unset() instead")
inline void unsetEnvironment(const std::string& key)
{
    unset(key);
}


#endif // YARP_NO_DEPRECATED

} // namespace environment
} // namespace conf
} // namespace yarp

#endif // YARP_CONF_ENVIRONMENT_H
