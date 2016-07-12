/*
 * Copyright (C) 2006 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */


#include <yarp/os/impl/UnitTest.h>
#include <yarp/os/Contact.h>

using namespace yarp::os::impl;
using namespace yarp::os;

class ContactTest : public UnitTest
{
public:
    virtual ConstString getName()
    {
        return "ContactTest";
    }

    virtual void testString()
    {
        report(0, "checking string representation");
        Contact address("tcp", "127.0.0.1", 10000);
        ConstString txt = address.toURI();
        checkEqual(txt, "tcp://127.0.0.1:10000", "string rep example");
    }

    virtual void testCopy()
    {
        report(0, "checking address copy");
        Contact address("tcp", "127.0.0.1", 10000);
        Contact address2;
        address2 = address;
        ConstString txt = address2.toURI();
        checkEqual(txt, "tcp://127.0.0.1:10000", "string rep example");

        Contact inv1;
        address2 = inv1;
        checkFalse(inv1.isValid(), "invalid source");
        checkFalse(address2.isValid(), "invalid copy");
    }

    virtual void testFactory()
    {
#ifndef YARP_NO_DEPRECATED // since YARP 2.3.68
        report(0, "checking deprecated static factory methods");
YARP_WARNING_PUSH;
YARP_DISABLE_DEPRECATED_WARNING;
        checkTrue(!Contact::invalid().isValid(), "good invalid");
        checkTrue(!Contact::empty().isValid(), "good invalid");
        {
            report(0, "checking deprecated static factory methods");
            Contact c = Contact::byName("/foo");
            checkFalse(c.isValid(), "invalid Contact");
            checkEqual(c.getHost().c_str(), "", "hostname not set");
            checkEqual(c.getPort(), -1, "port number not set");
            checkFalse(c.hasTimeout(), "timeout not set");
            checkEqual(c.getCarrier().c_str(), "", "carrier not set");
            checkEqual(c.getRegName().c_str(), "/foo", "regName set");
            checkEqual(c.getName().c_str(), "/foo", "port name generated");
        }
        {
            report(0, "checking deprecated static factory methods");
            Contact c = Contact::byCarrier("ziggy");
            checkFalse(c.isValid(), "invalid Contact");
            checkEqual(c.getHost().c_str(), "", "hostname not set");
            checkEqual(c.getPort(), -1, "port number not set");
            checkFalse(c.hasTimeout(), "timeout not set");
            checkEqual(c.getCarrier().c_str(), "ziggy", "carrier set");
            checkEqual(c.getRegName().c_str(), "", "regName not set");
            checkEqual(c.getName().c_str(), "", "port name not generated");
        }
        {
            report(0, "checking deprecated static factory methods");
            Contact c = Contact::bySocket("ziggy", "www.robotology.yarp", 8080);
            checkTrue(c.isValid(), "valid Contact");
            checkEqual(c.getHost().c_str(), "www.robotology.yarp", "hostname set");
            checkEqual(c.getPort(), 8080, "port number set");
            checkFalse(c.hasTimeout(), "timeout not set");
            checkEqual(c.getCarrier().c_str(), "ziggy", "carrier set");
            checkEqual(c.getRegName().c_str(), "", "regName not set");
            checkEqual(c.getName().c_str(), "/www.robotology.yarp:8080", "port name generated");
        }
YARP_WARNING_POP;

#endif // YARP_NO_DEPRECATED
    }

    virtual void testContact()
    {
        {
            report(0, "checking constructor with no parameters");
            Contact c;
            checkFalse(c.isValid(), "invalid Contact");
        }

        {
            report(0, "checking constructor with 1 parameter (name)");
            Contact c("/foo");
            checkFalse(c.isValid(), "not a valid Contact");
            checkEqual(c.getHost().c_str(), "", "hostname not set");
            checkEqual(c.getPort(), -1, "port number not set");
            checkFalse(c.hasTimeout(), "timeout not set");
            checkEqual(c.getCarrier().c_str(), "", "carrier not set");
            checkEqual(c.getRegName().c_str(), "/foo", "reg name set");
            checkEqual(c.getName().c_str(), "/foo", "port name set");
        }

        {
            report(0, "checking constructor with 2 parameters (name, carrier)");
            Contact c("/foo", "ziggy");
            checkFalse(c.isValid(), "not a valid Contact");
            checkEqual(c.getHost().c_str(), "", "hostname not set");
            checkEqual(c.getPort(), -1, "port number not set");
            checkFalse(c.hasTimeout(), "timeout not set");
            checkEqual(c.getCarrier().c_str(), "ziggy", "carrier set");
            checkEqual(c.getRegName().c_str(), "/foo", "reg name set");
            checkEqual(c.getName().c_str(), "/foo", "port name set");
        }

        {
            report(0, "checking constructor with 3 parameters (name, carrier, hostname)");
            Contact c("/foo", "ziggy", "www.robotology.yarp");
            checkFalse(c.isValid(), "not a valid Contact");
            checkEqual(c.getHost().c_str(), "www.robotology.yarp", "hostname set");
            checkEqual(c.getPort(), -1, "port number not set");
            checkFalse(c.hasTimeout(), "timeout not set");
            checkEqual(c.getCarrier().c_str(), "ziggy", "carrier set");
            checkEqual(c.getRegName().c_str(), "/foo", "reg name set");
            checkEqual(c.getName().c_str(), "/foo", "port name set");
        }

        {
            report(0, "checking constructor with 4 parameters (name, carrier, hostname, port)");
            Contact c("/foo", "ziggy", "www.robotology.yarp", 8080);
            checkTrue(c.isValid(), "valid Contact");
            checkEqual(c.getHost().c_str(), "www.robotology.yarp", "hostname set");
            checkEqual(c.getPort(), 8080, "port number set");
            checkFalse(c.hasTimeout(), "timeout not set");
            checkEqual(c.getCarrier().c_str(), "ziggy", "carrier set");
            checkEqual(c.getRegName().c_str(), "/foo", "reg name set");
            checkEqual(c.getName().c_str(), "/foo", "port name set");
        }

        {
            report(0, "checking hostname/port constructor");
            Contact c("www.robotology.yarp", 8080);
            checkTrue(c.isValid(), "valid Contact");
            checkEqual(c.getHost().c_str(), "www.robotology.yarp", "hostname set");
            checkEqual(c.getPort(), 8080, "port number set");
            checkFalse(c.hasTimeout(), "timeout not set");
            checkEqual(c.getCarrier().c_str(), "", "carrier not set");
            checkEqual(c.getRegName().c_str(), "", "reg name not set");
            checkEqual(c.getName().c_str(), "/www.robotology.yarp:8080", "port name generated");
        }

        {
            report(0, "checking hostname/port constructor with portnumber = 0");
            Contact c("www.robotology.yarp", 0);
            checkTrue(c.isValid(), "valid Contact");
            checkEqual(c.getHost().c_str(), "www.robotology.yarp", "hostname set");
            checkEqual(c.getPort(), 0, "port number set");
            checkFalse(c.hasTimeout(), "timeout not set");
            checkEqual(c.getCarrier().c_str(), "", "carrier not set");
            checkEqual(c.getRegName().c_str(), "", "reg name not set");
            checkEqual(c.getName().c_str(), "/www.robotology.yarp:0", "port name generated");
        }

        {
            report(0, "checking socket constructor");
            Contact c("ziggy", "www.robotology.yarp", 8080);
            checkTrue(c.isValid(), "valid Contact");
            checkEqual(c.getHost().c_str(), "www.robotology.yarp", "hostname set");
            checkEqual(c.getPort(), 8080, "port number set");
            checkFalse(c.hasTimeout(), "timeout not set");
            checkEqual(c.getCarrier().c_str(), "ziggy", "carrier set");
            checkEqual(c.getRegName().c_str(), "", "reg name not set");
            checkEqual(c.getName().c_str(), "/www.robotology.yarp:8080", "port name generated");
        }

        {
            report(0, "checking socket constructor with portnumber = 0");
            Contact c("ziggy", "www.robotology.yarp", 0);
            checkTrue(c.isValid(), "valid Contact");
            checkEqual(c.getHost().c_str(), "www.robotology.yarp", "hostname set");
            checkEqual(c.getPort(), 0, "port number set");
            checkFalse(c.hasTimeout(), "timeout not set");
            checkEqual(c.getCarrier().c_str(), "ziggy", "carrier set");
            checkEqual(c.getRegName().c_str(), "", "reg name not set");
            checkEqual(c.getName().c_str(), "/www.robotology.yarp:0", "port name generated");
        }

        {
            report(0, "checking copy assignment operator with invalid Contact");
            Contact c1;
            Contact c2;
            c1 = c2;
            checkFalse(c2.isValid(), "invalid copy");
        }

        {
            Contact c1;
            Contact c2("www.robotology.yarp", 8080);
            report(0, "checking copy assignment operator with valid Contact");
            c1 = c2;
            checkTrue(c1.isValid(), "valid Contact");
            checkEqual(c1.getHost().c_str(), "www.robotology.yarp", "hostname set");
            checkEqual(c1.getPort(), 8080, "port number set");
            checkFalse(c1.hasTimeout(), "timeout not set");
            checkEqual(c1.getCarrier().c_str(), "", "carrier not set");
            checkEqual(c1.getRegName().c_str(), "", "reg name not set");
            checkEqual(c1.getName().c_str(), "/www.robotology.yarp:8080", "port name generated");
        }

        {
            report(0, "checking copy constructor");
            Contact c1("www.robotology.yarp", 8080);
            Contact c2(c1);
            checkTrue(c2.isValid(), "valid Contact");
            checkEqual(c2.getHost().c_str(), "www.robotology.yarp", "hostname set");
            checkEqual(c2.getPort(), 8080, "port number set");
            checkFalse(c2.hasTimeout(), "timeout not set");
            checkEqual(c2.getCarrier().c_str(), "", "carrier not set");
            checkEqual(c2.getRegName().c_str(), "", "reg name not set");
            checkEqual(c2.getName().c_str(), "/www.robotology.yarp:8080", "port name generated");
        }

        {
            report(0, "checking move constructor");
            Contact c1("www.robotology.yarp", 8080);

#if YARP_COMPILER_CXX_RVALUE_REFERENCES
            Contact c2 = std::move(c1);
#else
            // On most compilers uses RVO => move constructor is not called
            Contact c2 = Contact("www.robotology.yarp", 8080);
#endif // YARP_COMPILER_CXX_RVALUE_REFERENCES
            checkTrue(c2.isValid(), "valid Contact");
            checkEqual(c2.getHost().c_str(), "www.robotology.yarp", "hostname set");
            checkEqual(c2.getPort(), 8080, "port number set");
            checkFalse(c2.hasTimeout(), "timeout not set");
            checkEqual(c2.getCarrier().c_str(), "", "carrier not set");
            checkEqual(c2.getRegName().c_str(), "", "reg name not set");
            checkEqual(c2.getName().c_str(), "/www.robotology.yarp:8080", "port name generated");
        }

        {
            report(0, "checking move assignment operator");
            Contact c;
            c = Contact("www.robotology.yarp", 8080);
            checkTrue(c.isValid(), "valid Contact");
            checkEqual(c.getHost().c_str(), "www.robotology.yarp", "hostname set");
            checkEqual(c.getPort(), 8080, "port number set");
            checkFalse(c.hasTimeout(), "timeout not set");
            checkEqual(c.getCarrier().c_str(), "", "carrier not set");
            checkEqual(c.getRegName().c_str(), "", "reg name not set");
            checkEqual(c.getName().c_str(), "/www.robotology.yarp:8080", "port name generated");
        }

        {
            report(0, "checking Contact wrapper on regular url");
            Contact c = Contact::fromString("http://www.robotology.yarp:8080/fuzz");
            checkTrue(c.isValid(), "valid Contact");
            checkEqual(c.getCarrier().c_str(), "http", "good carrier");
            checkEqual(c.getHost().c_str(), "www.robotology.yarp", "hostname set");
            checkEqual(c.getPort(), 8080, "port number set");
            checkEqual(c.getName().c_str(), "/fuzz", "good port name");
        }

        {
            report(0, "checking Contact wrapper on regular url without portnumber");
            Contact c = Contact::fromString("ziggy://my/url");
            checkFalse(c.isValid(), "valid Contact");
            checkEqual(c.getCarrier().c_str(), "ziggy", "good carrier");
//             checkEqual(c.getHost().c_str(), "my", "good host name");
            checkEqual(c.getPort(), -1, "port number set");
            checkEqual(c.getName().c_str(), "/my/url", "good port name");
        }

        {
            report(0, "checking Contact wrapper on minimal url");
            Contact c = Contact::fromString("/my/url2");
            checkFalse(c.isValid(), "valid Contact");
            checkEqual(c.getName().c_str(), "/my/url2", "good port name");
        }

        {
            report(0, "checking Contact wrapper on url without host");
            Contact c = Contact::fromString("ziggy:/my/url");
            checkFalse(c.isValid(), "valid Contact");
            checkEqual(c.getCarrier().c_str(), "ziggy", "good carrier");
            checkEqual(c.getName().c_str(), "/my/url", "good port name");
        }

        {
            report(0, "checking Contact wrapper on short url");
            Contact c = Contact::fromString("tcp://192.168.1.3:43705");
            checkTrue(c.isValid(), "valid Contact");
            checkEqual(c.getCarrier().c_str(), "tcp", "good carrier");
            checkEqual(c.getHost().c_str(), "192.168.1.3", "hostname set");
            checkEqual(c.getPort(), 43705, "port number set");
            checkEqual(c.getName().c_str(), "/192.168.1.3:43705", "good port name");
        }
    }

    virtual void runTests() {
        testString();
        testCopy();
        testFactory();
        testContact();
    }
};

static ContactTest theContactTest;

UnitTest& getContactTest() {
    return theContactTest;
}

