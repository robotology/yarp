// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-
#include <yarp/os/BinPortable.h>
#include <yarp/os/PortReaderBuffer.h>
#include <yarp/os/Port.h>
#include <yarp/NameClient.h>
#include <yarp/Companion.h>
#include <yarp/os/Time.h>

#include "TestList.h"

using namespace yarp;
using namespace yarp::os;

class BinPortableTest : public UnitTest {
public:
    virtual String getName() { return "BinPortableTest"; }

    void testInt() {
        report(0,"checking binary read/write of native int");
        BinPortable<int> i;
        i.content() = 5;

        PortReaderBuffer<BinPortable<int> > buf;
        Port input, output;
        input.open("/in");
        output.open("/out");
        input.setReader(buf);
        output.addOutput(Contact::byName("/in").addCarrier("tcp"));
        report(0,"writing...");
        output.write(i);
        report(0,"reading...");
        BinPortable<int> *result = buf.read();
        checkTrue(result!=NULL,"got something check");
        if (result!=NULL) {
            checkEqual(result->content(),5,"value preserved");
        }
        output.close();
        input.close();
    }

    virtual void runTests() {
        yarp::NameClient& nic = yarp::NameClient::getNameClient();
        nic.setFakeMode(true);
        testInt();
        nic.setFakeMode(false);
    }
};

static BinPortableTest theBinPortableTest;

UnitTest& getBinPortableTest() {
    return theBinPortableTest;
}
