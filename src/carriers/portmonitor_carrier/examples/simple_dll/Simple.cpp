// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright: (C) 2011 Department of Robotics Brain and Cognitive Sciences - Istituto Italiano di Tecnologia
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

#include "Simple.h"

#include <yarp/os/SharedLibraryClass.h>

// just for testing, we make two distinct copies of factory function
YARP_DEFINE_DEFAULT_SHARED_CLASS(AbstractMonitorObjectImpl);
YARP_DEFINE_SHARED_SUBCLASS(AbstractMonitorObject_there, AbstractMonitorObjectImpl, AbstractMonitorObject);


bool AbstractMonitorObjectImpl::setParams(const yarp::os::Property& params) 
{
    return true;
}

bool AbstractMonitorObjectImpl::getParams(yarp::os::Property& params)
{
    return true;
}

bool AbstractMonitorObjectImpl::hasAccept()
{
    return true;
}

bool AbstractMonitorObjectImpl::acceptData(yarp::os::Things& thing)
{
    return true;
}

bool AbstractMonitorObjectImpl::hasUpdate()
{
    return true;
}

yarp::os::Things& AbstractMonitorObjectImpl::updateData(yarp::os::Things& thing)
{
    return thing;
}

bool AbstractMonitorObjectImpl::peerTrigged(void)
{
    return true;
}


