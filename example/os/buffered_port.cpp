// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-
#include <yarp/os/Network.h>
#include <yarp/os/BufferedPort.h>
#include <yarp/os/Bottle.h>
#include <yarp/os/Time.h>
#include <stdio.h>

using namespace yarp::os;

int main() {
    Network::init();
    Network::setLocalMode(true); // don't actually need a name server
    {
    BufferedPort<Bottle> in;
    BufferedPort<Bottle> out;
    in.open("/in");
    out.open("/out");

    Network::connect("/out","/in");

    Bottle& outBot1 = out.prepare();
    outBot1.fromString("hello world");
    printf("Writing bottle 1 (%s)\n", outBot1.toString().c_str());
    out.write(true);

    Bottle& outBot2 = out.prepare();
    outBot2.fromString("2 3 5 7 11");
    printf("Writing bottle 2 (%s)\n", outBot2.toString().c_str());
    out.write(true);

    Bottle *inBot1 = in.readStrict();
    printf("Bottle 1 is: %s\n", inBot1->toString().c_str());
    Bottle *inBot2 = in.readStrict();
    printf("Bottle 2 is: %s\n", inBot2->toString().c_str());
    }
    Network::fini();
    return 0;
}

