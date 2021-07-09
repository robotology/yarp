/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/serversql/impl/LogComponent.h>

#include <yarp/os/Os.h>

#include <atomic>
#include <cstring>
namespace {
std::atomic<yarp::os::Log::LogType> minimumServersqlPrintLevel(yarp::os::Log::InfoType);
} // namespace

void yarp::serversql::impl::LogComponent::print_callback(yarp::os::Log::LogType type,
                                                         const char* msg,
                                                         const char* file,
                                                         const unsigned int line,
                                                         const char* func,
                                                         double systemtime,
                                                         double networktime,
                                                         double externaltime,
                                                         const char* comp_name)
{
    auto minlev = minimumServersqlPrintLevel.load();
    if (type >= minlev) {
        if (minlev <= yarp::os::Log::DebugType) {
            yarp::os::Log::printCallback()(type, msg, file, line, func, systemtime, networktime, externaltime, comp_name);
        } else {
            static const char* err_str = "[ERROR] ";
            static const char* warn_str = "[WARNING] ";
            static const char* no_str = "";
            printf("%s%s\n",
                   ((type == yarp::os::Log::ErrorType) ? err_str : ((type == yarp::os::Log::WarningType) ? warn_str : no_str)),
                   msg);
        }
    }
}

void yarp::serversql::impl::LogComponent::setMinumumLogType(yarp::os::Log::LogType minumumLogType)
{
    minimumServersqlPrintLevel = minumumLogType;
}
