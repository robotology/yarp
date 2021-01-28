/*
 * Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include <yarp/os/RFModule.h>
#include <yarp/os/Time.h>
#include <yarp/os/LogStream.h>

class moduleFoo : public yarp::os::RFModule
{
    virtual bool configure(yarp::os::ResourceFinder &r)
    {
        return true;
    }

    virtual bool updateModule() override
    {
        yarp::os::Time::delay(1);
        yDebug() << "Updated module Foo";
        return true;
    }
};
