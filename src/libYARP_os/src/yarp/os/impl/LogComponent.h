/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_OS_IMPL_LOGCOMPONENT_H
#define YARP_OS_IMPL_LOGCOMPONENT_H

#include <yarp/os/api.h>
#include <yarp/os/LogComponent.h>

namespace yarp::os::impl::LogComponent {

void print_callback(yarp::os::Log::LogType type,
                    const char* msg,
                    const char* file,
                    const unsigned int line,
                    const char* func,
                    double systemtime,
                    double networktime,
                    double externaltime,
                    const char* comp_name);

yarp::os::Log::LogType minimumLogType();
void setMinumumLogType(yarp::os::Log::LogType minumumLogType);

} // namespace yarp::os::impl::LogComponent

#define YARP_OS_LOG_COMPONENT(name, name_string) \
    const yarp::os::LogComponent& name() \
    { \
        static const yarp::os::LogComponent component(name_string, \
                                                      yarp::os::impl::LogComponent::minimumLogType(), \
                                                      yarp::os::Log::LogTypeReserved, \
                                                      yarp::os::Log::defaultPrintCallback(), \
                                                      nullptr); \
        return component; \
    }

#define YARP_OS_NON_CONST_LOG_COMPONENT(name, name_string) \
    yarp::os::LogComponent& name() \
    { \
        static yarp::os::LogComponent component(name_string, \
                                                yarp::os::impl::LogComponent::minimumLogType(), \
                                                yarp::os::Log::LogTypeReserved, \
                                                yarp::os::Log::defaultPrintCallback(), \
                                                nullptr); \
        return component; \
    }

#endif // YARP_OS_IMPL_LOGCOMPONENT_H
