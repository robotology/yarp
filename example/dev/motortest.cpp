// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright: (C) 2010 RobotCub Consortium
 * Authors: Paul Fitzpatrick, Giacomo Spigler
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

#include <yarp/os/Network.h>
#include <yarp/os/Port.h>
#include <yarp/os/Bottle.h>
#include <yarp/os/Time.h>
#include <stdio.h>
#include <stdlib.h>

#include <yarp/dev/PolyDriver.h>

#include <yarp/dev/ControlBoardInterfaces.h>


using namespace yarp::os;
using namespace yarp::dev;


int main(int argc, char *argv[]) {
    Network yarp;

    if(argc!=7) {
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
    options.put("board", config.check("board", yarp::os::Value("ssc32")).asString().c_str());
    options.put("comport", config.check("comport", yarp::os::Value("/dev/ttyS0")).asString().c_str());
    options.put("baudrate", config.check("baudrate", yarp::os::Value(38400)).asInt());


    PolyDriver dd(options);
    if(!dd.isValid()) {
      printf("Device not available.\n");
      Network::fini();
      return 1;
    }

    IPositionControl *pos;

    dd.view(pos);


    int jnts = 0;
    pos->getAxes(&jnts);

//printf("axes: %d\n", jnts);

//Time::delay(1);

    pos->positionMove(0, -45);


    Time::delay(1);



    dd.close();

    return 0;
}

