/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
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
