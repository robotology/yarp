/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef YARP_OS_IMPL_LOGFORWARDER_H
#define YARP_OS_IMPL_LOGFORWARDER_H

#include <yarp/os/api.h>
#include <yarp/os/BufferedPort.h>
#include <yarp/os/Network.h>
#include <yarp/os/Semaphore.h>
#include <string>

namespace yarp {
namespace os {

#define MAX_STRING_SIZE 255

class YARP_OS_API LogForwarder
{
    public:
        static LogForwarder* getInstance();
        static void clearInstance();
        void forward (const std::string& message);
    protected:
        LogForwarder();
        ~LogForwarder();
    private:
        static yarp::os::Semaphore *sem;
        char logPortName[MAX_STRING_SIZE];
        yarp::os::BufferedPort<yarp::os::Bottle>* outputPort;
    private:
        LogForwarder(LogForwarder const&){};
        LogForwarder& operator=(LogForwarder const&){return *this;}; //@@@checkme
        static LogForwarder* instance;
};

} // namespace os
} // namespace yarp

#endif // YARP_OS_IMPL_LOGFORWARDER_H
