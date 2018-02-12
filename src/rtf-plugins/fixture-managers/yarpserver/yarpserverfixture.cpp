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
#include <yarp/os/impl/NameClient.h>
#include <yarp/serversql/Server.h>
#define YSERVFIXT_TIMEOUT 10
using namespace yarp::os;
using namespace RTF;
//using namespace yarp::name;

class yarpServerThread : public yarp::os::Thread
{
    typedef yarp::serversql::Server Server;

    Server ys;
    int    argc;
    char** argv;

    virtual void run() override
    {
        ys.run(argc, argv);
    }

    virtual void onStop() override
    {
        ys.stop();
    }
public:
    virtual void configure(int inArgc, char** inArgv)
    {
        argc = inArgc;
        argv = inArgv;
    }
};

class YarpNameServer : public FixtureManager
{
public:
    yarp::os::Network net;
    yarpServerThread  yServer;

    virtual ~YarpNameServer(){ yServer.stop(); }


    virtual bool setup(int argc, char** argv) override
    {

        yServer.configure(argc, argv);
        yServer.start();
        double time = yarp::os::Time::now();
        while(!net.checkNetwork() || (yarp::os::Time::now() - time) < YSERVFIXT_TIMEOUT)
        {
            yarp::os::Time::delay(0.1);
        }
        return net.checkNetwork();
    }

    virtual bool check() override
    {
        return net.checkNetwork();
    }

    virtual void tearDown() override
    {

    }
};

PREPARE_FIXTURE_PLUGIN(YarpNameServer)
