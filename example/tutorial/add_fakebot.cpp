/*
 * Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include <yarp/os/Network.h>
#include <yarp/dev/Drivers.h>

#include <FakeBot.h>

using namespace yarp::os;
using namespace yarp::dev;

#define ENABLE_fakebot (1)

void add_fakebot() {
#if ENABLE_fakebot
    DriverCreator *factory =
        new DriverCreatorOf<FakeBot>("fakebot",
                                           "",
                                           "FakeBot");
    Drivers::factory().add(factory); // hand factory over to YARP
#endif
}
