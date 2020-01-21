/*
 * Copyright (C) 2006-2020 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef SIMPLE_INC
#define SIMPLE_INC

#include <yarp/os/Bottle.h>
#include <yarp/os/Things.h>
#include <yarp/os/MonitorObject.h>


class AsciiImageMonitorObject : public yarp::os::MonitorObject
{
public:
    bool create(const yarp::os::Property& options);
    void destroy(void);

    bool setparam(const yarp::os::Property& params);
    bool getparam(yarp::os::Property& params);

    bool accept(yarp::os::Things& thing);
    yarp::os::Things& update(yarp::os::Things& thing);

private:
    yarp::os::Bottle bt;
    yarp::os::Things th;
};

#endif
