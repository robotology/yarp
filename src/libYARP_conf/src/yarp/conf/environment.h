/*
 * Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */


#ifndef YARP_CONF_ENVIRONMENT_H
#define YARP_CONF_ENVIRONMENT_H

#include <yarp/conf/system.h>

#include <string>
#include <cstdlib>

namespace yarp {
namespace conf {
namespace environment {

#if defined(_WIN32)
static constexpr char path_separator = ';';
#else
static constexpr char path_separator = ':';
#endif


/**
 * Read a variable from the environment.
 *
 * @param key the variable to read
 * @param found an optional variable to set to true iff variable is found
 * @return the value of the environment variable, or "" if not found
 *
 * @since YARP 3.4
 */
inline std::string getEnvironment(const char* key, bool* found = nullptr)
{
    const char* result = std::getenv(key);
    if (found != nullptr) {
        *found = (result != nullptr);
    }
    if (result == nullptr) {
        return {};
    }
    return std::string(result);
}

/**
 * Set or change an environment variable.
 *
 * @param key the variable to set or change
 * @param val the target value
 *
 * @since YARP 3.4
 */
inline void setEnvironment(const std::string& key, const std::string& val)
{
#if defined(_MSC_VER)
    _putenv_s(key.c_str(), val.c_str());
#else
    ::setenv(key.c_str(), val.c_str(), 1);
#endif
}

/**
 * Remove an environment variable.
 *
 * @param key the variable to remove
 *
 * @since YARP 3.4
 */
inline void unsetEnvironment(const std::string& key)
{
#if defined(_MSC_VER)
    _putenv_s(key.c_str(), "");
#else
    ::unsetenv(key.c_str());
#endif
}


} // namespace filesystem
} // namespace conf
} // namespace yarp

#endif // YARP_CONF_ENVIRONMENT_H
