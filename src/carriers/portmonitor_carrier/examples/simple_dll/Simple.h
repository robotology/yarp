// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright: (C) 2011 Department of Robotics Brain and Cognitive Sciences - Istituto Italiano di Tecnologia
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

#ifndef SIMPLE_INC
#define SIMPLE_INC

#include "MonitorObject.h"

class SimpleMonitorObject : public MonitorObject 
{
public:
    bool create(void);
    void destroy(void);

    bool setparam(const yarp::os::Property& params);
    bool getparam(yarp::os::Property& params);

    bool trig(void);

    bool accept(yarp::os::Things& thing);
    yarp::os::Things& update(yarp::os::Things& thing);
};

#endif
