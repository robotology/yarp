/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/os/RFModule.h>

#include <yarp/os/ResourceFinder.h>
#include <yarp/os/Network.h>

#include <string>

#include <catch.hpp>
#include <harness.h>

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
        return RFModule::respond(command, reply);
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

TEST_CASE("os::RFModuleTest", "[yarp::os]")
{
    Network::setLocalMode(true);

    SECTION("Checking Port network responses")
    {

        MyModule mm;
        Port p1, p2;
        mm.attach(p2);
        bool ok1 = p1.open("/p1");
        bool ok2 = p2.open("/p2");
        REQUIRE(ok1); // /p1 opened ok
        REQUIRE(ok2); // /p2 opened ok
        Network::sync("/p1");
        Network::sync("/p2");
        Network::connect("/p1", "/p2");

        Bottle out;
        Bottle in;
        out.addInt32(42);
        p1.write(out, in);
        CHECK(in.get(0).asInt32() == out.get(0).asInt32()); // Port response
    }

    SECTION("Checking threaded RFModule...")
    {

        MyModule mm;
        ResourceFinder rf;

        CHECK(mm.joinModule() == true); // Module not threadified
        CHECK(mm.configure(rf) == true); // Configure completed
        CHECK(mm.runModuleThreaded() == 0); // Module threaded
        mm.stopModule();
        CHECK(mm.isStopping() == true); // Module stopping
        CHECK(mm.joinModule() == true); // Module threaded finished
    }

    Network::setLocalMode(false);
}
