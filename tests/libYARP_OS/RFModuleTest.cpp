/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include <string>
#include <yarp/os/all.h>
#include <yarp/os/ResourceFinder.h>
#include <yarp/os/RFModule.h>

#include <yarp/os/impl/UnitTest.h>
//#include "TestList.h"

using namespace yarp::os::impl;
using namespace yarp::os;

class MyModule : public RFModule
{
public:
    virtual bool respond(const Bottle& command, Bottle& reply) override
    {
        if (command.get(0).isInt32())
        {
            reply = command;
            return true;
        }
        return RFModule::respond(command,reply);
    }

    virtual bool configure(yarp::os::ResourceFinder &rf) override
    {
        return true;
    }

    virtual double getPeriod() override
    {
        return 1.0;
    }

    virtual bool updateModule() override
    {
        return true;
    }

};

class RFModuleTest : public UnitTest
{
public:
    virtual std::string getName() const override { return "RFModuleTest"; }


    void testPort()
    {
        report(0, "[Test] Checking Port network responses...");

        MyModule mm;
        Port p1, p2;
        mm.attach(p2);
        bool ok1 = p1.open("/p1");
        bool ok2 = p2.open("/p2");
        checkTrue(ok1 && ok2, "[Test] Ports opened ok");
        if (!(ok1 && ok2)) { return; }

        Network::connect("/p1","/p2");
        Network::sync("/p1");
        Network::sync("/p2");
        Bottle out, in;
        out.addInt32(42);
        p1.write(out,in);
        checkEqual(in.get(0).asInt32(), out.get(0).asInt32(), "[Test] Port response");
    }

    void testThread()
    {
        report(0, "[Test] Checking threaded RFModule...");

        MyModule mm;
        ResourceFinder rf;

        checkEqual(mm.joinModule(),        true, "[Test] Module not threadified");
        checkEqual(mm.configure(rf),       true, "[Test] Configure completed");
        checkEqual(mm.runModuleThreaded(), 0,    "[Test] Module threaded");
        mm.stopModule();
        checkEqual(mm.isStopping(),        true, "[Test] Module stopping");
        checkEqual(mm.joinModule(),        true, "[Test] Module threaded finished");
    }

    virtual void runTests() override
    {
        Network::setLocalMode(true);

        testPort();
        testThread();

        Network::setLocalMode(false);
    }
};

static RFModuleTest theRFModuleTest;

UnitTest& getRFModuleTest()
{
    return theRFModuleTest;
}
