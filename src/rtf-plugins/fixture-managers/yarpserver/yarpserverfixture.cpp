/*
 * Copyright: (C) 2017 Istituto Italiano di Tecnologia (IIT)
 * Authors: Andrea Ruzzenenti <andrea.ruzzenenti@iit.it>
 * Copy Policy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

#include <iostream>
#include <stdio.h>
#include <yarp/os/Thread.h>
#include <rtf/FixtureManager.h>
#include <rtf/dll/Plugin.h>
#include <yarp/serversql/yarpserversql.h>

using namespace yarp::os;
using namespace RTF;

class YarpServerThread : public yarp::os::Thread
{
    virtual void run() override
    {
        int ret = yarpserver_main(0, 0);
        YARP_UNUSED(ret);
    }
};

class YarpNameServer : public FixtureManager
{
public:
    YarpNameServer() {};
    virtual ~YarpNameServer() {};
    YarpServerThread yServer;
    virtual bool setup(int argc, char** argv) override
    {
        yServer.start();
        return true;
    }

    virtual void tearDown() override
    {

    }
};

PREPARE_FIXTURE_PLUGIN(YarpNameServer)
