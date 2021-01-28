/*
 * Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef YARP_SERVERSQL_IMPL_CONNECTTHREAD_H
#define YARP_SERVERSQL_IMPL_CONNECTTHREAD_H

#include <yarp/os/Thread.h>

#include <mutex>

namespace yarp {
namespace serversql {
namespace impl {

class ConnectThread : public yarp::os::Thread
{
public:
    bool needed {true};
    bool positive {true};
    int ct {0};
    std::mutex& mutex;

    std::string src;
    std::string dest;

    ConnectThread(std::mutex& mutex);

    void run() override;
};

} // namespace impl
} // namespace serversql
} // namespace yarp


#endif // YARP_SERVERSQL_IMPL_CONNECTTHREAD_H
