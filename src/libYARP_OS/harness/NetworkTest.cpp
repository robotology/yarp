// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2007 Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the GNU GPL v2.0.
 *
 */

#include <yarp/os/Network.h>
#include <yarp/os/Port.h>

#include "TestList.h"

using namespace yarp;
using namespace yarp::os;

class NetworkTest : public UnitTest {
public:
    virtual String getName() { return "NetworkTest"; }

    void checkConnect() {
        report(0,"checking return value of connect method");
        Port p1;
        Port p2;
        p1.open("/p1");
        p2.open("/p2");
        Network::sync("/p1");
        Network::sync("/p2");
        checkTrue(Network::connect("/p1","/p2"),"good connect");
        checkFalse(Network::connect("/p1","/p3"),"bad connect");
        p2.close();
        p1.close();
    }

    virtual void runTests() {
        Network::setLocalMode(true);
        checkConnect();
        Network::setLocalMode(false);
    }
};

static NetworkTest theNetworkTest;

UnitTest& getNetworkTest() {
    return theNetworkTest;
}
