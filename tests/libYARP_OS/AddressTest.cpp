/*
 * Copyright (C) 2006 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#include <yarp/os/Contact.h>

#include <yarp/os/impl/UnitTest.h>
//#include "TestList.h"

using namespace yarp::os::impl;
using namespace yarp::os;

class AddressTest : public UnitTest {
public:
    virtual String getName() { return "AddressTest"; }

    virtual void testString() {
        report(0,"checking string representation");
        Contact address = Contact::bySocket("tcp","127.0.0.1",10000);
        String txt = address.toURI();
        checkEqual(txt,"tcp://127.0.0.1:10000","string rep example");
    }

    virtual void testCopy() {
        report(0,"checking address copy");
        Contact address = Contact::bySocket("tcp","127.0.0.1",10000);
        Contact address2;
        address2 = address;
        String txt = address2.toURI();
        checkEqual(txt,"tcp://127.0.0.1:10000","string rep example");

        Contact inv1;
        address2 = inv1;
        checkTrue(!inv1.isValid(),"invalid source");
        checkTrue(!address2.isValid(),"invalid copy");
    }

    virtual void testContact() {
        report(0,"checking Contact");
        Contact c1;
        Contact inv1;
        c1 = inv1;
        checkTrue(!Contact::invalid().isValid(),"good invalid");
        checkTrue(!inv1.isValid(),"invalid source");
        checkTrue(!c1.isValid(),"invalid copy");

        report(0,"checking Contact wrapper on regular url");
        Contact c2 = Contact::fromString("http://www.google.yarp:8080/fuzz");
        checkEqual(c2.getCarrier().c_str(),"http","good carrier");
        checkEqual(c2.getHost().c_str(),"www.google.yarp","good hostname");
        checkEqual(c2.getPort(),8080,"good port number");
        checkEqual(c2.getName().c_str(),"/fuzz","good port name");

        report(0,"checking Contact wrapper on regular url without portnumber");
        Contact c3 = Contact::fromString("ziggy://my/url");
        checkEqual(c3.getCarrier().c_str(),"ziggy","good carrier");
        //checkEqual(c3.getHost().c_str(),"my","good host name");
        //checkEqual(c3.getPort(),-1,"good port number");
        checkEqual(c3.getName().c_str(),"/my/url","good port name");

        report(0,"checking Contact wrapper on minimal url");
        Contact c4 = Contact::fromString("/my/url2");
        checkEqual(c4.getName().c_str(),"/my/url2","good port name");

        report(0,"checking Contact wrapper on url without host");
        Contact c5 = Contact::fromString("ziggy:/my/url");
        checkEqual(c5.getCarrier().c_str(),"ziggy","good carrier");
        checkEqual(c5.getName().c_str(),"/my/url","good port name");

        report(0,"checking Contact wrapper on short url");
        Contact c6 = Contact::fromString("tcp://192.168.1.3:43705");
        checkEqual(c6.getCarrier().c_str(),"tcp","good carrier");
        checkEqual(c6.getHost().c_str(),"192.168.1.3","good hostname");
        checkEqual(c6.getPort(),43705,"good port number");
        checkEqual(c6.getName().c_str(),"/192.168.1.3:43705","good port name");
    }

    virtual void runTests() {
        testString();
        testCopy();
        testContact();
    }
};

static AddressTest theAddressTest;

UnitTest& getAddressTest() {
    return theAddressTest;
}

