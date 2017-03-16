/*
 * Copyright (C) 2006, 2008 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */


/*
 * Welcome to the "PopulateDrivers" source file, where factories
 * are created for YARP device drivers.
 */


#include <yarp/dev/Drivers.h>
#include <yarp/os/impl/Logger.h>
#include <yarp/os/Bottle.h>
#include <yarp/os/impl/PlatformStdio.h>
#include <yarp/os/impl/PlatformStdlib.h>
#include <yarp/dev/RemoteFrameGrabber.h>
#include <yarp/dev/ServerFrameGrabber.h>
#include <yarp/dev/DevicePipe.h>
#include <yarp/dev/DeviceGroup.h>
#include <yarp/dev/ServerSoundGrabber.h>
#include <yarp/dev/TestMotor.h>

using namespace yarp::os;
using namespace yarp::dev;

extern DriverCreator *createRemoteControlBoard();
extern DriverCreator *createAnalogSensorClient();
extern DriverCreator *createAnalogWrapper();
extern DriverCreator *createControlBoardWrapper();
extern DriverCreator *createVirtualAnalogWrapper();
extern DriverCreator *createServerInertial();
extern DriverCreator *createBatteryWrapper();
extern DriverCreator *createBatteryClient();
extern DriverCreator *createRangefinder2DWrapper();
extern DriverCreator *createRGBDSensorWrapper();
extern DriverCreator *createRGBDSensorClient();
extern DriverCreator *createControlBoardRemapper();
extern DriverCreator *createRemoteControlBoardRemapper();
extern DriverCreator *createNavigation2DClient();
extern DriverCreator *createRobotDescriptionServer();
extern DriverCreator *createRobotDescriptionClient();
extern DriverCreator *createServerGrabber();

#ifdef WITH_YARPMATH
extern DriverCreator *createFrameTransformServer();
extern DriverCreator *createFrameTransformClient();
extern DriverCreator *createLocationsServer();
extern DriverCreator *createRangefinder2DClient();
#endif

#ifndef YARP_NO_DEPRECATED
extern DriverCreator *createServerControlBoard();
extern DriverCreator *createClientControlBoard();
#endif // YARP_NO_DEPRECATED

void Drivers::init() {

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

    add(createRemoteControlBoard());
    add(createServerInertial());
    add(createControlBoardWrapper());
    add(createBatteryClient());
    add(createAnalogSensorClient());
    add(createAnalogWrapper());
    add(createVirtualAnalogWrapper());
    add(createBatteryWrapper());
    add(createRangefinder2DWrapper());
    add(createRGBDSensorWrapper());
    add(createRGBDSensorClient());
    add(createControlBoardRemapper());
    add(createRemoteControlBoardRemapper());
    add(createNavigation2DClient());
    add(createRobotDescriptionServer());
    add(createRobotDescriptionClient());
    add(createServerGrabber());

#ifdef WITH_YARPMATH
    add(createFrameTransformServer());
    add(createFrameTransformClient());
    add(createLocationsServer());
    add(createRangefinder2DClient());
#endif

#ifndef YARP_NO_DEPRECATED
    add(createClientControlBoard());
    add(createServerControlBoard());
#endif // YARP_NO_DEPRECATED
}
