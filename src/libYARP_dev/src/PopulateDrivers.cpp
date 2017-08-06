/*
 * Copyright (C) 2006, 2008 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */


/*
 * Welcome to the "PopulateDrivers" source file, where factoriesf
 * are created for YARP device drivers.
 */


#include <yarp/dev/Drivers.h>
#include <yarp/os/impl/Logger.h>
#include <yarp/os/Bottle.h>
#include <cstdio>
#include <cstdlib>
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
extern DriverCreator *createRobotDescriptionServer();
extern DriverCreator *createRobotDescriptionClient();
extern DriverCreator *createServerGrabber();
extern DriverCreator *createJoypadControlClient();
extern DriverCreator *createJoypadControlServer();

#ifdef WITH_YARPMATH
extern DriverCreator *createFrameTransformServer();
extern DriverCreator *createFrameTransformClient();
extern DriverCreator *createRangefinder2DClient();
extern DriverCreator *createNavigation2DClient();
extern DriverCreator *createLocalization2DClient();
extern DriverCreator *createMap2DClient();
extern DriverCreator *createMap2DServer();
#endif

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
    add(createRobotDescriptionServer());
    add(createRobotDescriptionClient());
    add(createServerGrabber());
    add(createJoypadControlServer());
    add(createJoypadControlClient());
#ifdef WITH_YARPMATH
    add(createFrameTransformServer());
    add(createFrameTransformClient());
    add(createRangefinder2DClient());
    add(createNavigation2DClient());
    add(createLocalization2DClient());
    add(createMap2DClient());
    add(createMap2DServer());
#endif

}
