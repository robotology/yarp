// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-
#include <yarp/Address.h>

#include "TestList.h"

using namespace yarp;

class AddressTest : public UnitTest {
public:
    virtual String getName() { return "AddressTest"; }

    virtual void testString() {
        report(0,"checking string representation");
        Address address("localhost",10000,"tcp");
        String txt = address.toString();
        checkEqual(txt,"tcp://localhost:10000","string rep example");
    }

    virtual void runTests() {
        testString();
    }
};

static AddressTest theAddressTest;

UnitTest& getAddressTest() {
    return theAddressTest;
}

