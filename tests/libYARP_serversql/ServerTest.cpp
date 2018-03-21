/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include <cstdio>
#include <cstdlib>

#include <yarp/os/all.h>
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
    virtual ConstString getName() override { return "ServerTest"; }

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
        checkTrue(addr1.getHost()==addr2.getHost(),"same machine");

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
        Contact addr1("tcp", "localhost", 9999);
        nic.registerName("/check/register/forced",addr1);
        Contact addr2 = nic.queryName("/check/register/forced");
        checkTrue(addr1.isValid(),"got an address");
        checkEqual(addr1.getHost(),addr2.getHost(),"same machine");
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
        Contact addr1("tcp", "192.168.1.100", 9998);
        nic.registerName("/check/list",addr1);
        ConstString result = nic.send("NAME_SERVER list",true);
        ConstString target = "registration name /check/list ip 192.168.1.100 port 9998 type tcp";
        checkTrue(result.find(target)!=ConstString::npos,"listing found");
    }

    void checkSetGet() {
        report(0,"checking set/get...");
        NameClient& nic = NameClient::getNameClient();
        nic.registerName("/check/set");
        ConstString result = nic.send("NAME_SERVER set /check/set prop val",true);
        result = nic.send("NAME_SERVER get /check/set prop",true);
        ConstString target = "port /check/set property prop = val";
        checkTrue(result.find(target)!=ConstString::npos,"answer found");
    }

    virtual void runTests() override {
        NetworkBase::setLocalMode(true);

        checkRegisterFree();
        checkRegisterForced();
        checkUnregister();
        checkPortRegister();
        checkList();
        checkSetGet();

        NetworkBase::setLocalMode(false);

    }
};


static ServerTest theServerTest;

UnitTest& getServerTest() {
    return theServerTest;
}
