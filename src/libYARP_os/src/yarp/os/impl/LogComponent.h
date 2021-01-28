/*
 * Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef YARP_OS_IMPL_LOGCOMPONENT_H
#define YARP_OS_IMPL_LOGCOMPONENT_H

#include <yarp/os/api.h>
#include <yarp/os/LogComponent.h>

namespace yarp {
namespace os {
namespace impl {
namespace LogComponent {

void print_callback(yarp::os::Log::LogType type,
                    const char* msg,
                    const char* file,
                    const unsigned int line,
                    const char* func,
                    double systemtime,
                    double networktime,
                    double externaltime,
                    const char* comp_name);

void setMinumumLogType(yarp::os::Log::LogType minumumLogType);

} // namespace LogComponent
} // namespace impl
} // namespace os
} // namespace yarp

#define YARP_OS_LOG_COMPONENT(name, name_string) \
    const yarp::os::LogComponent& name() \
    { \
        static const yarp::os::LogComponent component(name_string, \
                                                      yarp::os::Log::TraceType, \
                                                      yarp::os::Log::LogTypeReserved, \
                                                      yarp::os::impl::LogComponent::print_callback, \
                                                      nullptr); \
        return component; \
    }

#define YARP_OS_NON_CONST_LOG_COMPONENT(name, name_string) \
    yarp::os::LogComponent& name() \
    { \
        static yarp::os::LogComponent component(name_string, \
                                                yarp::os::Log::TraceType, \
                                                yarp::os::Log::LogTypeReserved, \
                                                yarp::os::impl::LogComponent::print_callback, \
                                                nullptr); \
        return component; \
    }

#endif // YARP_OS_IMPL_LOGCOMPONENT_H
