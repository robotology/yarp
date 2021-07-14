/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */


#include <yarp/run/Run.h>

#include <yarp/os/Network.h>
#include <yarp/os/Time.h>
#include <yarp/os/Thread.h>

#include <string>
#include <cstring>
#include <cstdio>

#include <catch.hpp>
#include <harness.h>

using namespace yarp::os::impl;
using namespace yarp::os;

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

        if (_argv) {
            delete[] _argv;
        }
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
        yarp::run::Run::main(_argc, _argv);
    }
};


TEST_CASE("run::RunTest", "[yarp::run]")
{

#if !defined(ENABLE_BROKEN_TESTS)
    YARP_SKIP_TEST("Skipping YARP_run broken tests")
#endif

    SECTION("testRun")
    {
        //this could be local or using an external nameserver, to be decided
        Network::setLocalMode(true);
        YarpRun runner;

        INFO("checking yarprun");

        const int argc=3;
        const char *argv[argc];
        argv[0]="dummy-name";
        argv[1]="--server";
        argv[2]="/run";

        runner.start(argc, (char**)argv);

        Time::delay(3);
        Property par;
        par.put("name", "testModule");


        std::string moduleTag="test_module";
        yarp::run::Run::start("/run", par, moduleTag);

        Time::delay(1);

        bool isRunning=yarp::run::Run::isRunning("/run", moduleTag);
        CHECK(isRunning); // isRunning
        fprintf(stderr, "done!\n");
        Network::setLocalMode(false);
    }
}
