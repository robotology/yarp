/*
 * Copyright: (C) 2010 RobotCub Consortium
 * Author: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
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

