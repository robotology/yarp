/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include <yarp/os/Terminator.h>
#include <yarp/os/impl/NameServer.h>
#include <yarp/os/Network.h>
#include <yarp/os/Time.h>

#include <yarp/os/impl/UnitTest.h>

using namespace yarp::os::impl;
using namespace yarp::os;

class TerminatorTest : public UnitTest {
public:
    virtual std::string getName() override { return "TerminatorTest"; }

    virtual void testTerminationPair() {
        report(0,"checking terminator connection");

        Network::setLocalMode(true);

        printf("registering port name: ");
        Terminee terminee("/tmp/quit");
        if (terminee.isOk())
            printf("ok\n");
        else {
            printf("failed\n");
            report(1,"failed to set terminator socket");
        }

        printf("sending quit message: ");
        if (Terminator::terminateByName("/tmp/quit"))
            printf("ok\n");
        else {
            printf("failed\n");
            report(1,"failed to set termination connection");
        }

        printf("quit flag was set properly: ");
        if (!terminee.mustQuit()) {
            printf("failed\n");
            report(1,"failed to receive the quit message");
        }
        else {
            printf("ok\n");
        }
    }

    virtual void runTests() override {
        testTerminationPair();
    }
};

static TerminatorTest theTerminatorTest;

UnitTest& getTerminatorTest() {
    return theTerminatorTest;
}
