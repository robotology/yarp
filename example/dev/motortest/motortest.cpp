/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/os/Network.h>
#include <yarp/os/Property.h>
#include <yarp/os/Time.h>

#include <yarp/dev/IPositionControl.h>
#include <yarp/dev/PolyDriver.h>

#include <cstdio>

using yarp::dev::IPositionControl;
using yarp::dev::PolyDriver;
using yarp::os::Network;
using yarp::os::Property;


int main(int argc, char* argv[])
{
    Network yarp;

    if (argc != 7) {
        printf("  Error: correct usage is %s --board BOARD --comport COM --baudrate BAUDRATE\n", argv[0]);
        printf("          BOARD is one out of ssc32, minissc, pontech_sv203x, mondotronic_smi, pololu_usb_16servo, picopic\n");
        printf("          COM is COMx or /dev/ttyS0\n");
        printf("          BAUDRATE is the baud rate, eg: 38400");

        return 1;
    }

    Property config;
    config.fromCommand(argc, argv);


    Property options;
    //options.put("robot", "icub"); // typically from the command line.
    options.put("device", "SerialServoBoard");
    options.put("board", config.check("board", yarp::os::Value("ssc32")).asString());
    options.put("comport", config.check("comport", yarp::os::Value("/dev/ttyS0")).asString());
    options.put("baudrate", config.check("baudrate", yarp::os::Value(38400)).asInt32());


    PolyDriver dd(options);
    if (!dd.isValid()) {
        printf("Device not available.\n");
        return 1;
    }

    IPositionControl* pos;
    dd.view(pos);

    int jnts = 0;
    pos->getAxes(&jnts);
    pos->positionMove(0, -45);

    yarp::os::Time::delay(1);

    dd.close();

    return 0;
}
