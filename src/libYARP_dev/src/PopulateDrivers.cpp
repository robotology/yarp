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

using namespace yarp::os;
using namespace yarp::dev;

extern DriverCreator* createTestMotor();
extern DriverCreator* createServerSerial();
extern DriverCreator* createRemoteFrameGrabber();
extern DriverCreator* createServerFrameGrabber();
extern DriverCreator* createDevicePipe();
extern DriverCreator* createDeviceGroup();
extern DriverCreator* createRemoteControlBoard();
extern DriverCreator* createAnalogSensorClient();
extern DriverCreator* createAnalogWrapper();
extern DriverCreator* createControlBoardWrapper();
extern DriverCreator* createVirtualAnalogWrapper();
extern DriverCreator* createServerInertial();
extern DriverCreator* createRGBDSensorWrapper();
extern DriverCreator* createRGBDSensorClient();
extern DriverCreator* createControlBoardRemapper();
extern DriverCreator* createRemoteControlBoardRemapper();
extern DriverCreator* createRobotDescriptionServer();
extern DriverCreator* createRobotDescriptionClient();
extern DriverCreator* createServerGrabber();
extern DriverCreator* createJoypadControlClient();
extern DriverCreator* createJoypadControlServer();

void Drivers::init() {
    add(createTestMotor());
    add(createServerSerial());
    add(createRemoteFrameGrabber());
    add(createServerFrameGrabber());
    add(createDevicePipe());
    add(createDeviceGroup());
    add(createRemoteControlBoard());
    add(createServerInertial());
    add(createControlBoardWrapper());
    add(createAnalogSensorClient());
    add(createAnalogWrapper());
    add(createVirtualAnalogWrapper());
    add(createRGBDSensorWrapper());
    add(createRGBDSensorClient());
    add(createControlBoardRemapper());
    add(createRemoteControlBoardRemapper());
    add(createRobotDescriptionServer());
    add(createRobotDescriptionClient());
    add(createServerGrabber());
    add(createJoypadControlServer());
    add(createJoypadControlClient());
}
