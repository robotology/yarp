// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2009 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#include <stdio.h>
#include <stdlib.h>

#include <yarp/os/all.h>
#include <yarp/os/impl/String.h>
#include <yarp/os/impl/NameClient.h>
#include <yarp/os/impl/UnitTest.h>
#include <yarp/os/impl/Companion.h>

using namespace yarp::os;
using namespace yarp::os::impl;

/**
 *
 * Name server regression tests.
 *
 */
class ServerTest : public UnitTest {
public:
    virtual String getName() { return "ServerTest"; }

    void checkRegisterFree() {
        report(0,"checking free register command...");
        NameClient& nic = NameClient::getNameClient();
        nic.registerName("/check/register1");
        Contact addr1 = nic.queryName("/check/register1");
        checkTrue(addr1.isValid(),"got an address");
        checkEqual(addr1.getCarrier().c_str(),"tcp","correct carrier");

        nic.registerName("/check/register2");
        Contact addr2 = nic.queryName("/check/register2");
        checkTrue(addr2.isValid(),"got a second address");
        checkTrue(addr1.getPort()!=addr2.getPort(),"different port number");
        checkTrue(addr1.getName()==addr2.getName(),"same machine");

        Contact addr3 = nic.queryName("/check/register1");
        checkTrue(addr3.isValid(),"first address still there");
        checkEqual(addr1.getPort(),addr3.getPort(),"same port number");

        Contact addr4 = nic.queryName("/check/register2");
        checkTrue(addr4.isValid(),"second address still there");
        checkEqual(addr2.getPort(),addr4.getPort(),"same port number");
    }

    void checkRegisterForced() {
        report(0,"checking forced register command...");

        NameClient& nic = NameClient::getNameClient();
        Contact addr1 = Contact::bySocket("tcp","localhost",9999);
        nic.registerName("/check/register/forced",addr1);
        Contact addr2 = nic.queryName("/check/register/forced");
        checkTrue(addr1.isValid(),"got an address");
        checkEqual(addr1.getName(),addr2.getName(),"same machine");
        checkEqual(addr1.getPort(),addr2.getPort(),"same port number");
        //Contact a2 = nic.queryName("/bar2");
        //checkEqual(a2.isValid(),false,"non-existent address");
    }

    void checkUnregister() {
        report(0,"checking unregister command...");
        NameClient& nic = NameClient::getNameClient();
        nic.registerName("/check/unregister");
        Contact addr1 = nic.queryName("/check/unregister");
        checkTrue(addr1.isValid(),"got an address");
        nic.unregisterName("/check/unregister");
        Contact addr2 = nic.queryName("/check/unregister");
        checkFalse(addr2.isValid(),"got no address");
    }

    void checkPortRegister() {
        report(0,"checking port registration...");
        NameClient& nic = NameClient::getNameClient();
        Port p;
        Contact addr1 = nic.queryName("/check/port");
        checkFalse(addr1.isValid(),"got an address");
        p.open("/check/port");
        Contact addr2 = nic.queryName("/check/port");
        checkTrue(addr2.isValid(),"got no address");
        p.close();
        Contact addr3 = nic.queryName("/check/port");
        checkFalse(addr3.isValid(),"got an address");
    }

    void checkList() {
        report(0,"checking list...");
        NameClient& nic = NameClient::getNameClient();
        Contact addr1 = Contact::bySocket("tcp","192.168.1.100",9998);
        nic.registerName("/check/list",addr1);
        String result = nic.send("NAME_SERVER list",true);
        String target = "registration name /check/list ip 192.168.1.100 port 9998 type tcp";
        checkTrue(result.find(target)!=String::npos,"listing found");
    }

    void checkSetGet() {
        report(0,"checking set/get...");
        NameClient& nic = NameClient::getNameClient();
        nic.registerName("/check/set");
        String result = nic.send("NAME_SERVER set /check/set prop val",true);
        result = nic.send("NAME_SERVER get /check/set prop",true);
        String target = "port /check/set property prop = val";
        checkTrue(result.find(target)!=String::npos,"answer found");
    }

    virtual void runTests() {
        checkRegisterFree();
        checkRegisterForced();
        checkUnregister();
        checkPortRegister();
        checkList();
        checkSetGet();
    }
};

int main(int argc, char *argv[]) {
    Network yarp;

    printf("Tests for a functioning name server\n");

    UnitTest::startTestSystem();
    ServerTest test;
    test.runTests();
    UnitTest::stopTestSystem();

    return 0;
}
