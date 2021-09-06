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

yarp::os::Log::LogType yarp::os::impl::LogComponent::minimumLogType()
{
    return minimumOsPrintLevel.load();
}

void yarp::os::impl::LogComponent::setMinumumLogType(yarp::os::Log::LogType minumumLogType)
{
    minimumOsPrintLevel = minumumLogType;
}
