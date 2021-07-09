/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/os/impl/LogComponent.h>

#include <yarp/os/Os.h>

#include <atomic>
#include <cstring>
#include <cstdlib>

namespace {

inline bool from_env(const char* name, bool defaultvalue)
{
    const char *strvalue = std::getenv(name);

    if(!strvalue) { return defaultvalue; }

    if(strcmp(strvalue, "1") == 0) { return true; }
    if(strcmp(strvalue, "true") == 0) { return true; }
    if(strcmp(strvalue, "True") == 0) { return true; }
    if(strcmp(strvalue, "TRUE") == 0) { return true; }
    if(strcmp(strvalue, "on") == 0) { return true; }
    if(strcmp(strvalue, "On") == 0) { return true; }
    if(strcmp(strvalue, "ON") == 0) { return true; }

    if(strcmp(strvalue, "0") == 0) { return false; }
    if(strcmp(strvalue, "false") == 0) { return false; }
    if(strcmp(strvalue, "False") == 0) { return false; }
    if(strcmp(strvalue, "FALSE") == 0) { return false; }
    if(strcmp(strvalue, "off") == 0) { return false; }
    if(strcmp(strvalue, "Off") == 0) { return false; }
    if(strcmp(strvalue, "OFF") == 0) { return false; }

    return defaultvalue;
}

std::atomic<bool> quiet(from_env("YARP_QUIET", false));
std::atomic<bool> verbose(from_env("YARP_VERBOSE", false) &&
                                !quiet.load());

std::atomic<yarp::os::Log::LogType> minimumOsPrintLevel(
    (quiet.load() ? yarp::os::Log::WarningType :
    (verbose.load() ? yarp::os::Log::DebugType : yarp::os::Log::InfoType)));

} // namespace

void yarp::os::impl::LogComponent::print_callback(yarp::os::Log::LogType type,
                                                  const char* msg,
                                                  const char* file,
                                                  const unsigned int line,
                                                  const char* func,
                                                  double systemtime,
                                                  double networktime,
                                                  double externaltime,
                                                  const char* comp_name)
{
    if (type >= minimumOsPrintLevel.load()) {
        yarp::os::Log::printCallback()(type, msg, file, line, func, systemtime, networktime, externaltime, comp_name);
    }
}

void yarp::os::impl::LogComponent::setMinumumLogType(yarp::os::Log::LogType minumumLogType)
{
    minimumOsPrintLevel = minumumLogType;
}
