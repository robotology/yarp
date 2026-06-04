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

#include <catch2/catch_amalgamated.hpp>
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
    {
        Thread::stop();
    }

    void run() override
    {
        yarp::run::Run::main(_argc, _argv);
    }
};


TEST_CASE("run::RunTest", "[yarp::run]")
{
    NetworkBase::setLocalMode(true);

    SECTION("testRun")
    {
        YarpRun runner;

        INFO("checking yarprun");

        const int argc=3;
        const char *argv[argc];
        argv[0]="dummy-name";
        argv[1]="--server";
        argv[2]="/run";

        runner.start(argc, (char**)argv);
        Time::delay(4);
        bool ret = true;

        ret = yarp::run::Run::isRunning("/run", "should_fail");
        CHECK(!ret);

        //start a process
        std::string str1 = "test_module1";
        Property par1;
        par1.put("name", "testModule");
        ret = yarp::run::Run::start("/run", par1, str1);
        CHECK(ret);

        yarp::os::Time::delay(1);

        // start another process
        std::string str2 = "test_module2";
        Property par2;
        par2.put("name", "testModule");
        ret = yarp::run::Run::start("/run", par2, str2);
        CHECK(ret);

        yarp::os::Time::delay(1);

        // check if the processes are running
        ret = yarp::run::Run::isRunning("/run", "test_module1");
        CHECK(ret);

        ret = yarp::run::Run::isRunning("/run", "test_module2");
        CHECK(ret);

        ret = yarp::run::Run::isRunning("/run", "nonExistingTag");
        CHECK(!ret);

        // now we have 2 processes running
        std::vector<yarp::run::Run::processInfo> processes;
        ret = yarp::run::Run::ps("/run",processes);
        CHECK(processes.size()==2);
        CHECK(ret);

        //stop 1 process
        ret = yarp::run::Run::sigterm("/run", "test_module1");
        CHECK(ret);

        yarp::os::Time::delay(1);

        // now we have only 1 process running
        std::vector<yarp::run::Run::processInfo> allprocessesclosed1;
        ret = yarp::run::Run::ps("/run",allprocessesclosed1);
        CHECK(allprocessesclosed1.size()==1);

        // stop all processes
        ret = yarp::run::Run::sigtermall("/run");
        CHECK(ret);

        yarp::os::Time::delay(1);

        // now we have no processes running
        std::vector<yarp::run::Run::processInfo> allprocessesclosed0;
        ret = yarp::run::Run::ps("/run",allprocessesclosed0);
        CHECK(allprocessesclosed0.size()==0);
        CHECK(ret);

        // check system info
        yarp::os::SystemInfoSerializer info;
        ret = yarp::run::Run::sysinfo("/run", info);
        CHECK(info.memory.totalSpace!=0);
        CHECK(ret);

        // terminate the server
        ret = yarp::run::Run::exit("/run");
        CHECK(ret);

        fprintf(stderr, "done!\n");

        runner.stop();
    }

    NetworkBase::setLocalMode(false);
}
