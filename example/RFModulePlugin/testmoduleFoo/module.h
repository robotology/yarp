/******************************************************************************
 *                                                                            *
 * Copyright (C) 2018 Fondazione Istituto Italiano di Tecnologia (IIT)        *
 * All Rights Reserved.                                                       *
 *                                                                            *
 ******************************************************************************/


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