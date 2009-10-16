// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006 Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the GNU GPL v2.0.
 *
 */

#include <yarp/os/impl/NameServer.h>
#include <yarp/os/impl/NameClient.h>
#include <yarp/os/impl/Companion.h>

#include <yarp/os/impl/UnitTest.h>
//#include "TestList.h"

using namespace yarp::os::impl;

class NameServerTest : public UnitTest {
public:
    virtual String getName() { return "NameServerTest"; }

    void checkRegister() {
        report(0,"checking register...");
        Address address("127.0.0.1",9999,"tcp");
        NameServer ns;
        ns.registerName("/foo",address);
        Address a1 = ns.queryName("/foo");
        checkEqual(a1.isValid(),true,"recover address");
        checkEqual(a1.getName(),"127.0.0.1","machine name matches");
        Address a2 = ns.queryName("/bar");
        checkEqual(a2.isValid(),false,"non-existent address");
    }

    void checkClientInterface() {
        report(0,"checking client interface...");
        NameClient& nic = NameClient::getNameClient();
        nic.setFakeMode(true);
        Address address("127.0.0.1",9999,"tcp");
        nic.registerName("/foo2",address);
        Address a1 = nic.queryName("/foo2");
        checkEqual(a1.isValid(),true,"recover address");
        checkEqual(a1.getName(),"127.0.0.1","machine name matches");
        Address a2 = nic.queryName("/bar2");
        checkEqual(a2.isValid(),false,"non-existent address");
        nic.setFakeMode(false);
    }

    void checkCompanion(bool fake) {
        report(0,"checking dud connections don't affect memory...");
        NameClient& nic = NameClient::getNameClient();
        nic.setFakeMode(fake);
        Address address("127.0.0.1",9999,"tcp");
        nic.registerName("/foo2",address);
        Companion::connect("/junk","/junk2",true);
        Companion::connect("/foo2","/junk2",true);
        for (int i=0; i<5; i++) {
            char buf[100];
            sprintf(buf,"/many/foo/%d", i);
            Companion::connect(buf,"/junk2",true);
        }
        nic.setFakeMode(false);
    }

    virtual void runTests() {
        checkRegister();
        checkClientInterface();
        checkCompanion(true);
        //checkCompanion(false);
    }
};

static NameServerTest theNameServerTest;

UnitTest& getNameServerTest() {
    return theNameServerTest;
}

