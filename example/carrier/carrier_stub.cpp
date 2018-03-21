/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include <stdio.h>
#include <yarp/os/all.h>

#include <yarp/os/Carrier.h>
#include <yarp/os/Carriers.h>

#include <yarp/os/impl/TextCarrier.h>

/**
 *
 * A minimal carrier variation - changes nothing except the first
 * 8-bytes of a regular text mode connection.
 *
 */
class TestCarrier : public yarp::os::impl::TextCarrier {
    
public:
    virtual yarp::os::ConstString getName() {
        return "test";
    }

    virtual yarp::os::ConstString getSpecifierName() {
        return "TESTTEST";
    }

    virtual Carrier *create() {
        return new TestCarrier();
    }
};

int main(int argc, char *argv[]) {
    yarp::os::Network yarp;
    yarp::os::Carriers::addCarrierPrototype(new TestCarrier);

    yarp::os::BufferedPort<yarp::os::Bottle> out, in;
    out.open("/test/out");
    in.open("/test/in");

    yarp::os::Network::connect("/test/out","/test/in","test");

    out.prepare().fromString("1 2 3");
    out.write();

    yarp::os::Bottle * bot = in.read();
    if (bot!=NULL) {
        printf("Got message %s\n", bot->toString().c_str());
    }

    out.close();
    in.close();

    return 0;
}


