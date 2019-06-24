/*
 * Copyright (C) 2006-2019 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef YARP_OS_IMPL_LOGFORWARDER_H
#define YARP_OS_IMPL_LOGFORWARDER_H

#include <yarp/os/api.h>

#include <yarp/os/Mutex.h>
#include <yarp/os/Port.h>

#include <string>

namespace yarp {
namespace os {
namespace impl {

class YARP_OS_impl_API LogForwarder
{
public:
    ~LogForwarder();
    static LogForwarder& getInstance();

    void forward(const std::string& message);
    static void shutdown();

private:
    LogForwarder();
    LogForwarder(LogForwarder const&) = delete;
    LogForwarder& operator=(LogForwarder const&) = delete;

    yarp::os::Mutex mutex;
    yarp::os::Port outputPort;
    static bool started;
};

} // namespace impl
} // namespace os
} // namespace yarp

#endif // YARP_OS_IMPL_LOGFORWARDER_H
