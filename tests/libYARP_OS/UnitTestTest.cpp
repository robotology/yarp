/*
 * Copyright (C) 2015 iCub Facility
 * Author: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

#include <yarp/os/Bottle.h>
#include <yarp/os/impl/UnitTest.h>

using namespace yarp::os;
using namespace yarp::os::impl;

class UnitTestTest : public UnitTest {
public:
    virtual ConstString getName() { return "UnitTestTest"; }

    void checkHeapMonitor() {
        if (!heapMonitorSupported()) {
            report(0, "skipping heap monitor check, requires YARP_TEST_HEAP to be set");
            return;
        }
        report(0, "check heap monitor is functional");

        // check some heap use is detected
        heapMonitorBegin();
        Bottle b("1 2 3");
        int ops = heapMonitorEnd();
        checkTrue(ops>0,"memory allocation used in parsing bottle");

        // check some ordinary operations pass
        Bottle b2("4 5 6");
        heapMonitorBegin();
        int a = 2 + b2.size();
        ops = heapMonitorEnd();
        checkEqual(a,5,"math works");
        checkTrue(ops==0,"no memory allocation used in adding two numbers");

        // check that when we promise no heap use, but it happens, then
        // we get an error
        UnitTest isolatedTest(NULL);
        report(0, "DO NOT PANIC about heap operation assertions below this line, it is part of the test");
        isolatedTest.heapMonitorBegin(false); // assert no memory allocation
        Bottle b3("1 2 3");                   // we lied!
        ops = isolatedTest.heapMonitorEnd();
        report(0, "DO NOT PANIC about heap operation assertions above this line, it is part of the test");
        checkTrue(ops>0,"memory allocation was detected");
        checkTrue(!isolatedTest.isOk(),"memory allocation was flagged as error");
    }

    virtual void runTests() {
        checkHeapMonitor();
    }
};

static UnitTestTest theUnitTestTest;

UnitTest& getUnitTestTest() {
    return theUnitTestTest;
}
