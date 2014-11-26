// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright: (C) 2011 Department of Robotics Brain and Cognitive Sciences - Istituto Italiano di Tecnologia
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

#include <stdio.h>
#include <yarp/os/Bottle.h>
#include <yarp/os/SharedLibraryClass.h>

#include "Simple.h"

using namespace yarp::os;


YARP_DEFINE_SHARED_SUBCLASS(MonitorObject_there, SimpleMonitorObject, MonitorObject);


bool SimpleMonitorObject::create(void)
{
   printf("created!\n"); 
   return true;
}

void SimpleMonitorObject::destroy(void)
{
    printf("destroyed!\n");
}

bool SimpleMonitorObject::setparam(const yarp::os::Property& params) 
{
    return true;
}

bool SimpleMonitorObject::getparam(yarp::os::Property& params)
{
    return true;
}

bool SimpleMonitorObject::accept(yarp::os::Things& thing)
{   
    Bottle* bt = thing.cast_as<Bottle>();
    if(bt->toString() == "ignore")
        return false;
    return true;
}

yarp::os::Things& SimpleMonitorObject::update(yarp::os::Things& thing)
{
    Bottle* bt = thing.cast_as<Bottle>();
    bt->addString("Modified in DLL");
    return thing;
}

bool SimpleMonitorObject::trig(void)
{
    return true;
}


