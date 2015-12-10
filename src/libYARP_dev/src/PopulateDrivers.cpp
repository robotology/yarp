// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Welcome to the "PopulateDrivers" source file, where factories
 * are created for YARP device drivers.
 *
 *
 */

/*
 * Copyright (C) 2006, 2008 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#include <yarp/dev/Drivers.h>
#include <yarp/os/impl/String.h>
#include <yarp/os/impl/Logger.h>
#include <yarp/os/Bottle.h>
#include <yarp/os/impl/PlatformStdio.h>
#include <yarp/os/impl/PlatformStdlib.h>
#include <yarp/dev/RemoteFrameGrabber.h>
#include <yarp/dev/ServerFrameGrabber.h>
#include <yarp/dev/DevicePipe.h>
#include <yarp/dev/DeviceGroup.h>
#include <yarp/dev/TestFrameGrabber.h>
#include <yarp/dev/ServerSoundGrabber.h>
#include <yarp/dev/TestMotor.h>

using namespace yarp::os;
using namespace yarp::dev;


extern DriverCreator *createRemoteControlBoard();
extern DriverCreator *createServerControlBoard();
extern DriverCreator *createClientControlBoard();
extern DriverCreator *createAnalogSensorClient();
extern DriverCreator *createAnalogWrapper();
extern DriverCreator *createControlBoardWrapper();
extern DriverCreator *createVirtualAnalogWrapper();
extern DriverCreator *createServerInertial();
extern DriverCreator *createBatteryWrapper();
extern DriverCreator *createBatteryClient();
extern DriverCreator *createRangefinder2DWrapper();
extern DriverCreator *createRangefinder2DClient();

void Drivers::init() {

    add(new DriverCreatorOf<yarp::dev::TestFrameGrabber>("test_grabber",
                                                         "grabber",
                                                         "yarp::dev::TestFrameGrabber"));

    add(new DriverCreatorOf<yarp::dev::TestMotor>("test_motor",
                                                  "controlboard",
                                                  "yarp::dev::TestMotor"));

    add(new DriverCreatorOf<yarp::dev::RemoteFrameGrabber>("remote_grabber",
                                                           "grabber",
                                                           "yarp::dev::RemoteFrameGrabber"));

    add(new DriverCreatorOf<yarp::dev::ServerFrameGrabber>("grabber",
                                                           "grabber",
                                                           "yarp::dev::ServerFrameGrabber"));




    add(new DriverCreatorOf<yarp::dev::ServerSoundGrabber>("sound_grabber",
                                                           "sound_grabber",
                                                           "yarp::dev::ServerSoundGrabber"));

    add(new DriverCreatorOf<yarp::dev::DevicePipe>("pipe",
                                                   "",
                                                   "yarp::dev::DevicePipe"));

    add(new DriverCreatorOf<yarp::dev::DeviceGroup>("group",
                                                    "",
                                                    "yarp::dev::DeviceGroup"));

    add(createServerInertial());
    add(createControlBoardWrapper());
    add(createRemoteControlBoard());
    add(createBatteryClient());
    add(createRangefinder2DClient());
    add(createAnalogSensorClient());
    add(createAnalogWrapper());
    add(createVirtualAnalogWrapper());
    add(createBatteryWrapper());
    add(createRangefinder2DWrapper());
    add(createClientControlBoard());
    add(createServerControlBoard());
}
