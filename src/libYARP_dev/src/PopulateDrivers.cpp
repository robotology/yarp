/*
 * Copyright (C) 2006-2019 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

/*
 * Welcome to the "PopulateDrivers" source file, where factoriesf
 * are created for YARP device drivers.
 */


#include <yarp/dev/Drivers.h>

using namespace yarp::os;
using namespace yarp::dev;

extern DriverCreator* createRobotDescriptionServer();
extern DriverCreator* createRobotDescriptionClient();
extern DriverCreator* createServerGrabber();
extern DriverCreator* createJoypadControlClient();
extern DriverCreator* createJoypadControlServer();

void Drivers::init() {
    add(createRobotDescriptionServer());
    add(createRobotDescriptionClient());
    add(createServerGrabber());
    add(createJoypadControlServer());
    add(createJoypadControlClient());
}
