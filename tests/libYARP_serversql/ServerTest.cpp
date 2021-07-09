/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <cstdio>
#include <cstdlib>

#include <yarp/os/Contact.h>
#include <yarp/os/Network.h>
#include <yarp/os/Port.h>
#include <yarp/os/impl/NameClient.h>

#include <catch.hpp>
#include <harness.h>

using namespace yarp::os;
using namespace yarp::os::impl;

/**
 * Name server regression tests.
 */
TEST_CASE("serversql::ServerTest", "[yarp::serversql]")
{
    Network::setLocalMode(true);

    SECTION("check register free")
    {
        INFO("checking free register command...");
        NameClient& nic = NameClient::getNameClient();
        nic.registerName("/check/register1");
        Contact addr1 = nic.queryName("/check/register1");
        CHECK(addr1.isValid()); // got an address
        CHECK(addr1.getCarrier() == "tcp"); // correct carrier

        nic.registerName("/check/register2");
        Contact addr2 = nic.queryName("/check/register2");
        CHECK(addr2.isValid()); // got a second address
        CHECK(addr1.getPort()!=addr2.getPort()); // different port number
        CHECK(addr1.getHost()==addr2.getHost()); // same machine

        Contact addr3 = nic.queryName("/check/register1");
        CHECK(addr3.isValid()); // first address still there
        CHECK(addr1.getPort() == addr3.getPort()); // same port number

        Contact addr4 = nic.queryName("/check/register2");
        CHECK(addr4.isValid()); // second address still there
        CHECK(addr2.getPort() == addr4.getPort()); // same port number
    }

    SECTION("check register forced")
    {
        INFO("checking forced register command...");
        NameClient& nic = NameClient::getNameClient();
        Contact addr1("tcp", "localhost", 9999);
        nic.registerName("/check/register/forced",addr1);
        Contact addr2 = nic.queryName("/check/register/forced");
        CHECK(addr1.isValid()); // got an address
        CHECK(addr1.getHost() == addr2.getHost()); // same machine
        CHECK(addr1.getPort() == addr2.getPort()); // same port number
        //Contact a2 = nic.queryName("/bar2");
        //CHECK(a2.isValid() == false); // non-existent address
    }

    SECTION("check unregister")
    {
        INFO("checking unregister command...");
        NameClient& nic = NameClient::getNameClient();
        nic.registerName("/check/unregister");
        Contact addr1 = nic.queryName("/check/unregister");
        CHECK(addr1.isValid()); // got an address
        nic.unregisterName("/check/unregister");
        Contact addr2 = nic.queryName("/check/unregister");
        CHECK_FALSE(addr2.isValid()); // got no address
    }

    SECTION("check port register")
    {
        INFO("checking port registration...");
        NameClient& nic = NameClient::getNameClient();
        Port p;
        Contact addr1 = nic.queryName("/check/port");
        CHECK_FALSE(addr1.isValid()); // got an address
        p.open("/check/port");
        Contact addr2 = nic.queryName("/check/port");
        CHECK(addr2.isValid()); // got no address
        p.close();
        Contact addr3 = nic.queryName("/check/port");
        CHECK_FALSE(addr3.isValid()); // got an address
    }

    SECTION("check list")
    {
        INFO("checking list...");
        NameClient& nic = NameClient::getNameClient();
        Contact addr1("tcp", "192.168.1.100", 9998);
        nic.registerName("/check/list",addr1);
        std::string result = nic.send("NAME_SERVER list",true);
        std::string target = "registration name /check/list ip 192.168.1.100 port 9998 type tcp";
        CHECK(result.find(target)!=std::string::npos); // listing found
    }

    SECTION("check set get")
    {
        INFO("checking set/get...");
        NameClient& nic = NameClient::getNameClient();
        nic.registerName("/check/set");
        std::string result = nic.send("NAME_SERVER set /check/set prop val",true);
        result = nic.send("NAME_SERVER get /check/set prop",true);
        std::string target = "port /check/set property prop = val";
        CHECK(result.find(target)!=std::string::npos); // answer found
    }

    Network::setLocalMode(false);
}
