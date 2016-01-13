/*
 * Copyright (C) 2006 RobotCub Consortium
 * Authors: Giorgio Metta
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
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
    virtual String getName() { return "TerminatorTest"; }

    virtual void testTerminationPair() {
        report(0,"checking terminator connection");

        Network::setLocalMode(true);

        ACE_OS::printf("registering port name: ");
        Terminee terminee("/tmp/quit");
        if (terminee.isOk())
            ACE_OS::printf("ok\n");
        else {
            ACE_OS::printf("failed\n");
            report(1,"failed to set terminator socket");
        }

        ACE_OS::printf("sending quit message: ");
        if (Terminator::terminateByName("/tmp/quit"))
            ACE_OS::printf("ok\n");
        else {
            ACE_OS::printf("failed\n");
            report(1,"failed to set termination connection");
        }

        ACE_OS::printf("quit flag was set properly: ");
        if (!terminee.mustQuit()) {
            ACE_OS::printf("failed\n");
            report(1,"failed to receive the quit message");
        }
        else {
            ACE_OS::printf("ok\n");
        }
    }

    virtual void runTests() {
        testTerminationPair();
    }
};

static TerminatorTest theTerminatorTest;

UnitTest& getTerminatorTest() {
    return theTerminatorTest;
}
