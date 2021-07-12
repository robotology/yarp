/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

//! [buffered_port buffered_port.cpp]
#include <yarp/os/Bottle.h>
#include <yarp/os/BufferedPort.h>
#include <yarp/os/Network.h>

#include <cstdio>

using yarp::os::Bottle;
using yarp::os::BufferedPort;
using yarp::os::Network;

int main(int argc, char* argv[])
{
    YARP_UNUSED(argc);
    YARP_UNUSED(argv);

    // Initialize YARP - some OSes need network and time service initialization
    Network yarp;

    // Work locally - don't rely on name server (just for this example).
    // If you have a YARP name server running, you can remove this line.
    Network::setLocalMode(true);

    // Create two ports that we'll be using to transmit "Bottle" objects.
    // The ports are buffered, so that sending and receiving can happen
    // in the background.
    BufferedPort<Bottle> in;
    BufferedPort<Bottle> out;

    // we will want to read every message, with no skipping of "old" messages
    // when new ones come in
    in.setStrict();

    // Name the ports
    in.open("/in");
    out.open("/out");

    // Connect the ports so that anything written from /out arrives to /in
    Network::connect("/out", "/in");

    // Send one "Bottle" object.  The port is responsible for creating
    // and reusing/destroying that object, since it needs to be sure
    // it exists until communication to all recipients (just one in
    // this case) is complete.
    Bottle& outBot1 = out.prepare();   // Get the object
    outBot1.fromString("hello world"); // Set it up the way we want
    printf("Writing bottle 1 (%s)\n", outBot1.toString().c_str());
    out.write(); // Now send it on its way

    // Send another "Bottle" object
    Bottle& outBot2 = out.prepare();
    outBot2.fromString("2 3 5 7 11");
    printf("Writing bottle 2 (%s)\n", outBot2.toString().c_str());
    out.writeStrict(); // writeStrict() will wait for any
                       // previous communication to finish;
                       // write() would skip sending if
                       // there was something being sent

    // Read the first object
    Bottle* inBot1 = in.read();
    printf("Bottle 1 is: %s\n", inBot1->toString().c_str());

    // Read the second object
    Bottle* inBot2 = in.read();
    printf("Bottle 2 is: %s\n", inBot2->toString().c_str());

    return 0;
}
//! [buffered_port buffered_port.cpp]
