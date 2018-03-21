/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include <yarp/os/Network.h>
#include <yarp/os/Time.h>
#include <yarp/os/Run.h>
#include <yarp/os/Thread.h>

#include <string>
#include <cstring>
#include <cstdio>

#include <yarp/os/impl/UnitTest.h>

using namespace yarp::os::impl;
using namespace yarp::os;

#ifdef main
#undef main
#endif

class YarpRun: public Thread
{
    int _argc;
    char **_argv;
public:
    YarpRun()
    {
        _argc=0;
        _argv=nullptr;
    }
    ~YarpRun()
    {
        for(int a=0;a<_argc;a++)
        {
            delete [] _argv[a];
        }

        if (_argv)
            delete [] _argv;
        _argv=nullptr;
    }

    void start(int argc, char **argv)
    {
        _argc = argc;
        _argv=new char *[argc];
        for(int a=0;a<argc;a++)
        {
            _argv[a]=new char [strlen(argv[a])+1];
            strcpy(_argv[a], argv[a]);
        }

        Thread::start();
    }

    void stop()
    {}

    void run() override
    {
        yarp::os::Run::main(_argc, _argv);
    }
};


class RunTest : public UnitTest {
public:
    virtual ConstString getName() override { return "RunTest"; }

    virtual void testRun() {
        //this could be local or using an external nameserver, to be decided
        Network::setLocalMode(false);
        Network yarp;
        YarpRun runner;

        report(0,"checking yarprun");

        const int argc=3;
        const char *argv[argc];
        argv[0]="dummy-name";
        argv[1]="--server";
        argv[2]="/run";

        runner.start(argc, (char**)argv);

        Time::delay(3);
        Property par;
        par.put("name", "testModule");


        ConstString moduleTag="test_module";
        yarp::os::Run::start("/run", par, moduleTag);

        Time::delay(1);

        bool isRunning=yarp::os::Run::isRunning("/run", moduleTag);
        checkTrue(isRunning,"isRunning");

    //    terminate("/prova");

        fprintf(stderr, "done!\n");
        //Time::delay(10);
        //Run
        //NetworkBase::exists(""
        checkTrue(false,"test ok");
    }

    void terminate(const std::string &server)
    {
        Port tmpPort;
        tmpPort.open("...");
        Network::connect(tmpPort.getName().c_str(), "/run");

        Bottle msg, reply;
        msg.fromString("(exit)");
        tmpPort.write(msg, reply);
    }

    virtual void runTests() override {
        testRun();
    }
};

static RunTest theRunTest;

UnitTest& getRunTest() {
    return theRunTest;
}

