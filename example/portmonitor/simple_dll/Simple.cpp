/*
 * Copyright (C) 2014 iCub Facility
 * Authors: Ali Paikan
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

#include <stdio.h>
#include <yarp/os/Bottle.h>
#include <yarp/os/SharedLibraryClass.h>

#include "Simple.h"

using namespace yarp::os;


YARP_DEFINE_SHARED_SUBCLASS(MonitorObject_there, SimpleMonitorObject, MonitorObject);


bool SimpleMonitorObject::create(const yarp::os::Property& options)
{
   printf("created!\n"); 
   printf("I am attached to the %s\n",
          (options.find("sender_side").asBool()) ? "sender side" : "receiver side");
   return true;
}

void SimpleMonitorObject::destroy(void)
{
    printf("destroyed!\n");
}

bool SimpleMonitorObject::setparam(const yarp::os::Property& params) 
{
    return false;
}

bool SimpleMonitorObject::getparam(yarp::os::Property& params)
{
    return false;
}

bool SimpleMonitorObject::accept(yarp::os::Things& thing)
{   
    Bottle* bt = thing.cast_as<Bottle>();
    if(bt == NULL) {
        printf("SimpleMonitorObject: expected type Bottle but got wrong data type!\n");
        return false;
    }

    if(bt->toString() == "ignore")
        return false;
    return true;
}

yarp::os::Things& SimpleMonitorObject::update(yarp::os::Things& thing)
{
    Bottle* bt = thing.cast_as<Bottle>();
    if(bt == NULL) {
        printf("SimpleMonitorObject: expected type Bottle but got wrong data type!\n");
        return thing;
    }

    bt->addString("Modified in DLL");
    return thing;
}

void SimpleMonitorObject::trig(void)
{

}


