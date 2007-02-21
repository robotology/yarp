// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-


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

