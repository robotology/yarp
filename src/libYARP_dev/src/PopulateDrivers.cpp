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
#include <yarp/dev/ServerInertial.h>
#include <yarp/dev/TestFrameGrabber.h>
#include <yarp/dev/ServerSoundGrabber.h>
#include <yarp/dev/TestMotor.h>

using namespace yarp::os;
using namespace yarp::dev;


extern DriverCreator *createRemoteControlBoard();
extern DriverCreator *createServerControlBoard();
extern DriverCreator *createAnalogSensorClient();

#ifdef USE_NEW_WRAPPERS_WIP
    extern DriverCreator *createAnalogWrapper();
    extern DriverCreator *createControlBoardWrapper();
    extern DriverCreator *createVirtualAnalogWrapper();
#endif
void Drivers::init() {

    add(new DriverCreatorOf<TestFrameGrabber>("test_grabber",
                                              "grabber",
                                              "TestFrameGrabber"));

    add(new DriverCreatorOf<TestMotor>("test_motor",
                                       "controlboard",
                                       "TestMotor"));

    add(new DriverCreatorOf<RemoteFrameGrabber>("remote_grabber",
                                                "grabber",
                                                "RemoteFrameGrabber"));

    add(new DriverCreatorOf<ServerFrameGrabber>("grabber",
                                                "grabber",
                                                "ServerFrameGrabber"));


    add(new DriverCreatorOf<ServerInertial>("inertial",
                                            "inertial",
                                            "ServerInertial"));
                                            
    add(new DriverCreatorOf<ServerSoundGrabber>("sound_grabber",
                                                "sound_grabber",
                                                "ServerSoundGrabber"));
						
    add(new DriverCreatorOf<DevicePipe>("pipe",
                                        "",
                                        "DevicePipe"));

    add(new DriverCreatorOf<DeviceGroup>("group",
                                        "",
                                        "DeviceGroup"));

    add(createRemoteControlBoard());
    add(createServerControlBoard());
    add(createAnalogSensorClient());
#ifdef USE_NEW_WRAPPERS_WIP
    add(createAnalogWrapper());
    add(createControlBoardWrapper());
    add(createVirtualAnalogWrapper());
#endif
}
