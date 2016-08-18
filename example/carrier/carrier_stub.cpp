/*
 * Copyright: (C) 2010 RobotCub Consortium
 * Author: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

#include <stdio.h>
#include <yarp/os/all.h>

#include <yarp/os/impl/Carrier.h>
#include <yarp/os/Carriers.h>
#include <yarp/os/impl/TextCarrier.h>

using namespace yarp::os;
using namespace yarp::os::impl;

/**
 *
 * A minimal carrier variation - changes nothing except the first
 * 8-bytes of a regular text mode connection.
 *
 */
class TestCarrier : public TextCarrier {
public:
    virtual String getName() {
        return "test";
    }

    virtual String getSpecifierName() {
        return "TESTTEST";
    }

    virtual Carrier *create() {
        return new TestCarrier();
    }
};

int main(int argc, char *argv[]) {
    Network yarp;
    Carriers::addCarrierPrototype(new TestCarrier);

    BufferedPort<Bottle> out, in;
    out.open("/test/out");
    in.open("/test/in");

    Network::connect("/test/out","/test/in","test");

    out.prepare().fromString("1 2 3");
    out.write();

    Bottle * bot = in.read();
    if (bot!=NULL) {
        printf("Got message %s\n", bot->toString().c_str());
    }

    out.close();
    in.close();

    return 0;
}


