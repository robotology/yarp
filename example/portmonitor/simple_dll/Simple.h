/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef SIMPLE_INC
#define SIMPLE_INC

#include <yarp/os/MonitorObject.h>

class SimpleMonitorObject : public yarp::os::MonitorObject
{
public:
    bool create(const yarp::os::Property& options);
    void destroy();

    bool setparam(const yarp::os::Property& params);
    bool getparam(yarp::os::Property& params);

    void trig();

    bool accept(yarp::os::Things& thing);
    yarp::os::Things& update(yarp::os::Things& thing);
};

#endif
