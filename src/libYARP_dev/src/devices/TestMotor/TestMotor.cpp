/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include "TestMotor.h"

#include <yarp/dev/Drivers.h>

// needed for the driver factory.
yarp::dev::DriverCreator *createTestMotor() {
    return new yarp::dev::DriverCreatorOf<yarp::dev::TestMotor>("test_motor",
                                                                "controlboard",
                                                                "yarp::dev::TestMotor");
}
