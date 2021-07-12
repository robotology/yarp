/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "Simple.h"

#include <yarp/os/Bottle.h>
#include <yarp/os/Things.h>
#include <yarp/os/Log.h>


using namespace yarp::os;


bool SimpleMonitorObject::create(const yarp::os::Property& options)
{
   yDebug("created!\n");
   yDebug("I am attached to the %s\n",
          (options.find("sender_side").asBool()) ? "sender side" : "receiver side");
   return true;
}

void SimpleMonitorObject::destroy()
{
    yDebug("destroyed!\n");
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
        yWarning("SimpleMonitorObject: expected type Bottle but got wrong data type!\n");
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
        yWarning("SimpleMonitorObject: expected type Bottle but got wrong data type!\n");
        return thing;
    }

    bt->addString("Modified in DLL");
    return thing;
}

void SimpleMonitorObject::trig()
{
}
