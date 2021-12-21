/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_PORTMONITOR_RPCMONITOR_H
#define YARP_PORTMONITOR_RPCMONITOR_H

#include <yarp/os/MonitorObject.h>

#include <yarp/os/Port.h>

#include <string>

class RpcMonitor :
        public yarp::os::MonitorObject
{
public:
    bool create(const yarp::os::Property& options) override;
    yarp::os::Things& update(yarp::os::Things& thing) override;
    yarp::os::Things& updateReply(yarp::os::Things& thing) override;

private:
    bool sender;
    std::string source;
    std::string destination;
    yarp::os::Port port;
};

#endif // YARP_PORTMONITOR_RPCMONITOR_H
