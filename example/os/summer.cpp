/*
 * Copyright (C) 2006-2020 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include <yarp/os/Bottle.h>
#include <yarp/os/BufferedPort.h>
#include <yarp/os/LogStream.h>
#include <yarp/os/Network.h>
#include <iostream>

int main(int argc, char *argv[])
{
    yarp::os::Network yarp;
    yarp::os::BufferedPort<yarp::os::Bottle> port;
    port.open("/summer");
    while (true) {
        yInfo() << "waiting for input";
        yarp::os::Bottle *input = port.read();
        if (input != nullptr) {
            yInfo() << "got " << input->toString().c_str();
            double total = 0;
            for (int i=0; i<input->size(); i++) {
                total += input->get(i).asFloat64();
            }
            yarp::os::Bottle& output = port.prepare();
            output.clear();
            output.addString("total");
            output.addFloat64(total);
            yInfo() << "writing " << output.toString().c_str();
            port.write();
        }
    }
    return 0;
}
