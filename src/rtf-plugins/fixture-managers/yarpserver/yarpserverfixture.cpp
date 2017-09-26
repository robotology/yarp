/*
 * Copyright: (C) 2017 Istituto Italiano di Tecnologia (IIT)
 * Authors: Andrea Ruzzenenti <andrea.ruzzenenti@iit.it>
 * Copy Policy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

#include <iostream>
#include <stdio.h>
#include <yarp/os/Thread.h>
#include <yarp/os/Network.h>
#include <rtf/FixtureManager.h>
#include <rtf/dll/Plugin.h>
#include <yarp/serversql/yarpserversql.h>
#include <yarp/os/LogStream.h>

using namespace yarp::os;
using namespace RTF;

class YarpServerThread : public yarp::os::Thread
{
public:
    void configure(int inArgc, char** inArgv)
    {
        argc = inArgc;
        argv = inArgv;
    }
private:
    int    argc;
    char** argv;
    virtual void run() override
    {
        int ret = yarpserver_main(argc, argv);
        YARP_UNUSED(ret);
    }
};

class YarpNameServer : public FixtureManager
{
public:
    yarp::os::Network net;
    YarpNameServer() {};
    virtual ~YarpNameServer() {};
    YarpServerThread yServer;

    virtual bool setup(int argc, char** argv) override
    {
        yServer.configure(argc, argv);
        yServer.start();
        for (int i = 0; i < 10; i++)
        {
            if(net.checkNetwork()) return true;
            yarp::os::Time::delay(0.2);
        }
        yError() << "name server is taking too long to start.. aborting";
        return false;
    }

    virtual void tearDown() override
    {

    }
};

PREPARE_FIXTURE_PLUGIN(YarpNameServer)
