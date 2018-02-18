/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include <yarp/os/Election.h>
#include <yarp/os/impl/UnitTest.h>
//#include "TestList.h"

using namespace yarp::os;
using namespace yarp::os::impl;

class ElectionTest : public UnitTest {
public:
    virtual ConstString getName() override { return "ElectionTest"; }

    void testBasics() {
        report(0,"testing the basics of elections");
        ConstString c1 = "Magnifico";
        ConstString c2 = "Grasso";
        ConstString c3 = "Bozo";
        ElectionOf<PeerRecord<ConstString> > elector;
        elector.add("italy",&c1);
        elector.add("italy",&c2);
        elector.add("france",&c3);
        ConstString *e1 = elector.getElect("italy");
        ConstString *e2 = elector.getElect("france");
        checkTrue(e1!=nullptr,"elected entity exists (1)");
        checkTrue(e2!=nullptr,"elected entity exists (2)");
        if (e1!=nullptr && e2!=nullptr) {
            checkTrue((*e1==c1 || *e1==c2),"elected entity is accurate (1)");
            checkTrue(*e2==c3,"elected entity is accurate (2)");
        }
        elector.remove("italy",&c2);
        elector.remove("france",&c3);
        e1 = elector.getElect("italy");
        e2 = elector.getElect("france");
        checkTrue(e1!=nullptr,"elected entity exists (1)");
        checkTrue(e2==nullptr,"elected entity does not exist (2)");
        if (e1!=nullptr) {
            checkTrue(*e1==c1,"elected entity is accurate (1)");
        }
    }

    virtual void runTests() override {
        testBasics();
    }
};

static ElectionTest theElectionTest;

UnitTest& getElectionTest() {
    return theElectionTest;
}

