// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2007 Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the GNU GPL v2.0.
 *
 */


#include <yarp/String.h>
#include <yarp/os/all.h>
#include <yarp/os/Module.h>

#include "TestList.h"

using namespace yarp;
using namespace yarp::os;

class MyModule : public Module {
public:
    virtual bool respond(const Bottle& command, Bottle& reply) {
        if (command.get(0).isInt()) {
            reply = command;
            return true;
        }
        return Module::respond(command,reply);
    }
};

class ModuleTest : public UnitTest {
public:
    virtual String getName() { return "ModuleTest"; }


    void testPort() {
        report(0,"checking Port network responses...");

        MyModule mm;
        Port p1, p2;
        mm.attach(p2);
        p1.open("/p1");
        p2.open("/p2");
        Network::connect("/p1","/p2");
        Network::sync("/p1");
        Network::sync("/p2");
        Bottle out, in;
        out.addInt(42);
        p1.write(out,in);
        checkEqual(in.get(0).asInt(),out.get(0).asInt(),"Port response");
    }

    void testBufferedPort() {
        report(0,"checking BufferedPort network responses...");

        MyModule mm;
        Port p1;
        BufferedPort<Bottle> p2;
        mm.attach(p2);
        p1.open("/p1");
        p2.open("/p2");
        Network::connect("/p1","/p2");
        Network::sync("/p1");
        Network::sync("/p2");
        Bottle out, in;
        out.addInt(42);
        p1.write(out,in);
        checkEqual(in.get(0).asInt(),out.get(0).asInt(),
                   "BufferdPort response");
    }

    virtual void runTests() {
        Network::setLocalMode(true);
        testPort();
        testBufferedPort();
        Network::setLocalMode(false);
    }
};

static ModuleTest theModuleTest;

UnitTest& getModuleTest() {
    return theModuleTest;
}
