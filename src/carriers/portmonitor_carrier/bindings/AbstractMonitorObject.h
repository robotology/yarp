// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2012 IITRBCS
 * Authors: Ali Paikan
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifndef _ABSTRACT_MONITOR_OBJECT_INC_
#define _ABSTRACT_MONITOR_OBJECT_INC_

#include <string>
#include <yarp/os/ConstString.h>
#include <yarp/os/Property.h>
#include <yarp/os/Things.h>

class AbstractMonitorObject
{
public:
    virtual bool setParams(const yarp::os::Property& params) = 0;
    virtual bool getParams(yarp::os::Property& params) = 0;
    
    virtual bool hasAccept() = 0;
    virtual bool acceptData(yarp::os::Things& thing) = 0;

    virtual bool hasUpdate() = 0;
    virtual yarp::os::Things& updateData(yarp::os::Things& thing) = 0;

    virtual bool peerTrigged(void) = 0;
};

#endif //_ABSTRACT_MONITOR_OBJECT_INC_

