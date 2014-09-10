// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2012 IITRBCS
 * Authors: Ali Paikan
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifndef _MONITOR_OBJECT_INC_
#define _MONITOR_OBJECT_INC_

#include <string>
#include <yarp/os/ConstString.h>
#include <yarp/os/Property.h>
#include <yarp/os/Things.h>

class MonitorObject
{
public:
    virtual bool create(void) { return true; }
    virtual void destroy(void) { }

    virtual bool setparam(const yarp::os::Property& params) { }
    virtual bool getparam(yarp::os::Property& params) { }
   
    virtual bool trig(void) { }

    virtual bool hasAccept() = 0;
    virtual bool accept(yarp::os::Things& thing) { return true; }

    virtual bool hasUpdate() = 0;
    virtual yarp::os::Things& update(yarp::os::Things& thing) { return thing; }

 };

#endif //_MONITOR_OBJECT_INC_

