/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include <yarp/os/impl/NameServer.h>
#include <yarp/os/impl/Companion.h>

#include <yarp/os/impl/UnitTest.h>
//#include "TestList.h"

#include <yarp/os/Network.h>

using namespace yarp::os::impl;
using namespace yarp::os;

class NameServerTest : public UnitTest {
public:
    int safePort() {
        return Network::getDefaultPortRange()+100;
    }

    virtual ConstString getName() override { return "NameServerTest"; }

    void checkRegister() {
        report(0,"checking register...");
        Contact address("tcp", "127.0.0.1", safePort());
        NameServer ns;
        ns.registerName("/foo",address);
        Contact a1 = ns.queryName("/foo");
        checkEqual(a1.isValid(),true,"recover address");
        checkEqual(a1.getHost(),"127.0.0.1","machine name matches");
        Contact a2 = ns.queryName("/bar");
        checkEqual(a2.isValid(),false,"non-existent address");
    }

    void checkClientInterface() {
        report(0,"checking client intrface...");
        NetworkBase::setLocalMode(true);
        Contact address("/foo2", "tcp", "127.0.0.1", safePort());
        NetworkBase::registerContact(address);
        Contact a1 = NetworkBase::queryName("/foo2");
        checkEqual(a1.isValid(),true,"recover address");
        checkEqual(a1.getHost().c_str(),"127.0.0.1","machine name matches");
        Contact a2 = NetworkBase::queryName("/bar2");
        checkEqual(a2.isValid(),false,"non-existent address");
        NetworkBase::setLocalMode(false);
    }

    void checkCompanion(bool fake) {
        report(0,"checking dud connections don't affect memory...");
        NetworkBase::setLocalMode(fake);
        Contact address("/foo2", "tcp", "127.0.0.1", safePort());
        NetworkBase::registerContact(address);
        NetworkBase::connect("/junk","/junk2",nullptr,true);
        NetworkBase::connect("/foo2","/junk2",nullptr,true);
        for (int i=0; i<5; i++) {
            char buf[100];
            sprintf(buf,"/many/foo/%d", i);
            NetworkBase::connect(buf,"/junk2",nullptr,true);
        }
        NetworkBase::setLocalMode(false);
    }

    virtual void runTests() override {
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

