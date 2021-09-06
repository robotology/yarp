/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/os/impl/LogComponent.h>

#include <yarp/conf/environment.h>

#include <atomic>
#include <cstring>
#include <cstdlib>

namespace {

std::atomic<bool> quiet(yarp::conf::environment::get_bool("YARP_QUIET", false));
std::atomic<bool> verbose(yarp::conf::environment::get_bool("YARP_VERBOSE", false) && !quiet.load());

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
