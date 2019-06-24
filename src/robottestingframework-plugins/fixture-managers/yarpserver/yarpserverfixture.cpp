/*
 * Copyright (C) 2006-2019 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include <iostream>
#include <stdio.h>
#include <yarp/os/Thread.h>
#include <yarp/os/Network.h>
#include <robottestingframework/FixtureManager.h>
#include <robottestingframework/dll/Plugin.h>
#include <yarp/serversql/yarpserversql.h>
#include <yarp/os/LogStream.h>
#include <yarp/os/impl/NameClient.h>
#include <yarp/serversql/Server.h>
#define YSERVFIXT_TIMEOUT 10
using namespace yarp::os;
using namespace robottestingframework;
//using namespace yarp::name;

class yarpServerThread : public yarp::os::Thread
{
    typedef yarp::serversql::Server Server;

    Server ys;
    int    argc;
    char** argv;

    void run() override
    {
        ys.run(argc, argv);
    }

    void onStop() override
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


    bool setup(int argc, char** argv) override
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

    bool check() override
    {
        return net.checkNetwork();
    }

    void tearDown() override
    {

    }
};

ROBOTTESTINGFRAMEWORK_PREPARE_FIXTURE_PLUGIN(YarpNameServer)
