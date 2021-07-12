/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */


#include <yarp/os/RFModule.h>
#include <yarp/os/Time.h>
#include <yarp/os/LogStream.h>

class moduleBar : public yarp::os::RFModule
{
    virtual bool configure(yarp::os::ResourceFinder &r)
    {
        return true;
    }

    virtual bool updateModule() override
    {
        yarp::os::Time::delay(1);
        yDebug() << "Updated module Bar";
        return true;
    }
};
