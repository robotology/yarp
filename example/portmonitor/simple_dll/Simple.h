/*
 * Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
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
