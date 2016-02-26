/*
 * Copyright (C) 2014 iCub Facility
 * Authors: Ali Paikan
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

#ifndef SIMPLE_INC
#define SIMPLE_INC

#include <yarp/os/MonitorObject.h>

class SimpleMonitorObject : public yarp::os::MonitorObject 
{
public:
    bool create(const yarp::os::Property& options);
    void destroy(void);

    bool setparam(const yarp::os::Property& params);
    bool getparam(yarp::os::Property& params);

    void trig(void);

    bool accept(yarp::os::Things& thing);
    yarp::os::Things& update(yarp::os::Things& thing);
};

#endif
