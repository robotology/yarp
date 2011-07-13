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

using namespace yarp::os::impl;
using namespace yarp::os;
using namespace yarp::dev;


// NONE of this stuff is relevant anymore -- begins

void addExternalDevices();

//#ifndef YARP_AUTOCONF
//#cmakedefine EMBED_DEVICE_LIBRARY_CALL
//#cmakedefine CREATE_DEVICE_LIBRARY_MODULES
//#cmakedefine MERGE_DEVICE_LIBRARY_MODULES
//#endif

#ifdef CREATE_DEVICE_LIBRARY_MODULES_DISABLE
#ifdef MERGE_DEVICE_LIBRARY_MODULES
extern "C" void add_yarpmod_devices();
#endif
#endif

// NONE of this stuff is relevant anymore -- ends



#include <yarp/dev/RemoteFrameGrabber.h>
#include <yarp/dev/ServerFrameGrabber.h>
#include <yarp/dev/DevicePipe.h>
#include <yarp/dev/DeviceGroup.h>
#include <yarp/dev/ServerInertial.h>
#include <yarp/dev/TestFrameGrabber.h>
#include <yarp/dev/ServerSoundGrabber.h>
#include <yarp/dev/TestMotor.h>

extern DriverCreator *createRemoteControlBoard();
extern DriverCreator *createServerControlBoard();
extern DriverCreator *createAnalogSensorClient();

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

#ifndef YARP_AUTOCONF
#ifdef EMBED_DEVICE_LIBRARY_CALL
    addExternalDevices();
#endif
#endif

#ifdef CREATE_DEVICE_LIBRARY_MODULES
#ifdef MERGE_DEVICE_LIBRARY_MODULES
    add_yarpmod_devices();
#endif
#endif
}
