/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/os/Contact.h>

#include <catch.hpp>
#include <harness.h>

using namespace yarp::os;
TEST_CASE("os::ContactTest", "[yarp::os]")
{
    SECTION("checking string representation")
    {
        Contact address("tcp", "127.0.0.1", 10000);
        std::string txt = address.toURI();
        CHECK(txt == "tcp://127.0.0.1:10000/"); // string rep example
    }

    SECTION("checking address copy")
    {
        Contact address("tcp", "127.0.0.1", 10000);
        Contact address2;
        address2 = address;
        std::string txt = address2.toURI();
        CHECK(txt ==  "tcp://127.0.0.1:10000/"); // string rep example
        Contact inv1;
        address2 = inv1;
        CHECK(!inv1.isValid()); // invalid source
        CHECK(!address2.isValid()); // invalid copy
    }

    SECTION("checking constructor with no parameters")
    {
        Contact c;
        CHECK_FALSE(c.isValid()); // invalid Contact
    }

    SECTION("checking constructor with 1 parameter (name)")
    {
        Contact c("/foo");
        CHECK_FALSE(c.isValid()); // not a valid Contact
        CHECK(c.getHost().empty()); // hostname not set
        CHECK(c.getPort() == -1); // port number not set
        CHECK_FALSE(c.hasTimeout()); // timeout not set
        CHECK(c.getCarrier().empty()); // carrier not set
        CHECK(c.getRegName() == "/foo"); // reg name set
        CHECK(c.getName() == "/foo"); // port name set
    }

    SECTION("checking constructor with 2 parameters (name, carrier)")
    {
        Contact c("/foo", "ziggy");
        CHECK_FALSE(c.isValid()); // not a valid Contact
        CHECK(c.getHost().empty()); // hostname not set
        CHECK(c.getPort() == -1); // port number not set
        CHECK_FALSE(c.hasTimeout()); // timeout not set
        CHECK(c.getCarrier() == "ziggy"); // carrier set
        CHECK(c.getRegName() == "/foo"); // reg name set
        CHECK(c.getName() == "/foo"); // port name set
    }

    SECTION("checking constructor with 3 parameters (name, carrier, hostname)")
    {
        Contact c("/foo", "ziggy", "www.robotology.yarp");
        CHECK_FALSE(c.isValid()); // not a valid Contact
        CHECK(c.getHost() == "www.robotology.yarp"); // hostname set
        CHECK(c.getPort() == -1); // port number not set
        CHECK_FALSE(c.hasTimeout()); // timeout not set
        CHECK(c.getCarrier() == "ziggy"); // carrier set
        CHECK(c.getRegName() == "/foo"); // reg name set
        CHECK(c.getName() == "/foo"); // port name set
    }

    SECTION("checking constructor with 4 parameters (name, carrier, hostname, port)")
    {
        Contact c("/foo", "ziggy", "www.robotology.yarp", 8080);
        CHECK(c.isValid()); // valid Contact
        CHECK(c.getHost() == "www.robotology.yarp"); // hostname set
        CHECK(c.getPort() == 8080); // port number set
        CHECK_FALSE(c.hasTimeout()); // timeout not set
        CHECK(c.getCarrier() == "ziggy"); // carrier set
        CHECK(c.getRegName() == "/foo"); // reg name set
        CHECK(c.getName() == "/foo"); // port name set
    }

    SECTION("checking hostname/port constructor")
    {
        Contact c("www.robotology.yarp", 8080);
        CHECK(c.isValid()); // valid Contact
        CHECK(c.getHost() == "www.robotology.yarp"); //  hostname set
        CHECK(c.getPort() == 8080); // port number set
        CHECK_FALSE(c.hasTimeout()); // timeout not set
        CHECK(c.getCarrier().empty()); // carrier not set
        CHECK(c.getRegName().empty()); // reg name not set
        CHECK(c.getName() == "/www.robotology.yarp:8080"); // port name generated
    }

    SECTION("checking hostname/port constructor with portnumber = 0")
    {
        Contact c("www.robotology.yarp", 0);
        CHECK(c.isValid()); // valid Contact
        CHECK(c.getHost() == "www.robotology.yarp"); // hostname set
        CHECK(c.getPort() == 0); // port number set
        CHECK_FALSE(c.hasTimeout()); // timeout not set
        CHECK(c.getCarrier().empty()); //  carrier not set
        CHECK(c.getRegName().empty()); // reg name not set
        CHECK(c.getName() == "/www.robotology.yarp:0"); // port name generated
    }

    SECTION("checking socket constructor")
    {
        Contact c("ziggy", "www.robotology.yarp", 8080);
        CHECK(c.isValid()); // valid Contact
        CHECK(c.getHost() == "www.robotology.yarp"); // hostname set
        CHECK(c.getPort() == 8080); //  port number set
        CHECK_FALSE(c.hasTimeout()); //  timeout not set
        CHECK(c.getCarrier() == "ziggy"); // carrier set
        CHECK(c.getRegName().empty()); // reg name not set
        CHECK(c.getName() == "/www.robotology.yarp:8080"); //  "port name generated");
    }

    SECTION("checking socket constructor with portnumber = 0")
    {
        Contact c("ziggy", "www.robotology.yarp", 0);
        CHECK(c.isValid()); // valid Contact
        CHECK(c.getHost() == "www.robotology.yarp"); //  hostname set
        CHECK(c.getPort() == 0); // port number set
        CHECK_FALSE(c.hasTimeout()); // timeout not set
        CHECK(c.getCarrier() == "ziggy"); // carrier set
        CHECK(c.getRegName().empty()); // reg name not set
        CHECK(c.getName() == "/www.robotology.yarp:0"); // port name generated
    }

    SECTION("checking copy assignment operator with invalid Contact")
    {
        Contact c1;
        Contact c2;
        c1 = c2;
        CHECK_FALSE(c2.isValid()); // invalid copy
    }

    SECTION("checking copy assignment operator with valid Contact")
    {
        Contact c1;
        Contact c2("www.robotology.yarp", 8080);
        c1 = c2;
        CHECK(c1.isValid()); // valid Contact
        CHECK(c1.getHost() == "www.robotology.yarp"); // hostname set
        CHECK(c1.getPort() == 8080); // port number set
        CHECK_FALSE(c1.hasTimeout()); // timeout not set
        CHECK(c1.getCarrier().empty()); // carrier not set
        CHECK(c1.getRegName().empty()); // reg name not set
        CHECK(c1.getName() == "/www.robotology.yarp:8080"); // port name generated
    }

    SECTION("checking copy constructor")
    {
        Contact c1("www.robotology.yarp", 8080);
        Contact c2(c1);
        CHECK(c2.isValid()); // valid Contact
        CHECK(c2.getHost() == "www.robotology.yarp"); // hostname set
        CHECK(c2.getPort() == 8080); // port number set
        CHECK_FALSE(c2.hasTimeout()); // timeout not set
        CHECK(c2.getCarrier().empty()); // carrier not set
        CHECK(c2.getRegName().empty()); // reg name not set
        CHECK(c2.getName() == "/www.robotology.yarp:8080"); // port name generated
    }

    SECTION("checking move constructor")
    {
        Contact c1("www.robotology.yarp", 8080);
        Contact c2 = std::move(c1);
        CHECK(c2.isValid()); // valid Contact
        CHECK(c2.getHost() == "www.robotology.yarp"); // hostname set
        CHECK(c2.getPort() == 8080); // port number set
        CHECK_FALSE(c2.hasTimeout()); // timeout not set
        CHECK(c2.getCarrier().empty()); // carrier not set
        CHECK(c2.getRegName().empty()); // reg name not set
        CHECK(c2.getName() == "/www.robotology.yarp:8080"); // port name generated
    }

    SECTION("checking move assignment operator")
    {
        Contact c;
        c = Contact("www.robotology.yarp", 8080);
        CHECK(c.isValid()); // valid Contact
        CHECK(c.getHost() == "www.robotology.yarp"); // hostname set
        CHECK(c.getPort() == 8080); // port number set
        CHECK_FALSE(c.hasTimeout()); // timeout not set
        CHECK(c.getCarrier().empty()); // carrier not set
        CHECK(c.getRegName().empty()); // reg name not set
        CHECK(c.getName() == "/www.robotology.yarp:8080"); // port name generated
    }

    SECTION("checking Contact wrapper on regular url")
    {
        Contact c = Contact::fromString("http://www.robotology.yarp:8080/fuzz");
        CHECK(c.isValid()); // valid Contact
        CHECK(c.getCarrier() == "http"); // good carrier
        CHECK(c.getHost() == "www.robotology.yarp"); // hostname set
        CHECK(c.getPort() == 8080); // port number set
        CHECK(c.getName() == "/fuzz"); // good port name
    }

    SECTION("checking Contact wrapper on regular url without portnumber")
    {
        Contact c = Contact::fromString("ziggy://my/url");
        CHECK_FALSE(c.isValid()); // valid Contact
        CHECK(c.getCarrier() == "ziggy"); // good carrier
        CHECK(c.getPort() == -1); // port number set
        CHECK(c.getName() == "/my/url"); // good port name
    }

    SECTION("checking Contact wrapper on minimal url")
    {
        Contact c = Contact::fromString("/my/url2");
        CHECK_FALSE(c.isValid()); // valid Contact
        CHECK(c.getName() == "/my/url2"); // good port name
    }

    SECTION("checking Contact wrapper on url without host")
    {
        Contact c = Contact::fromString("ziggy:/my/url");
        CHECK_FALSE(c.isValid()); // valid Contact
        CHECK(c.getCarrier() == "ziggy"); // good carrier
        CHECK(c.getName() == "/my/url"); // good port name
    }

    SECTION("checking Contact wrapper on short url")
    {
        Contact c = Contact::fromString("tcp://192.168.1.3:43705");
        CHECK(c.isValid()); // valid Contact
        CHECK(c.getCarrier() == "tcp"); // good carrier
        CHECK(c.getHost() == "192.168.1.3"); // hostname set
        CHECK(c.getPort() == 43705); // port number set
        CHECK(c.getName() == "/192.168.1.3:43705"); // good port name
    }
}
