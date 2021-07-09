/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <cstdio>
#include <yarp/os/Time.h>
#include <yarp/os/Network.h>

static int __custom_yarp_is_initialized = 0;

// customizable initialization and shutdown functions

#ifdef yarpcar_INIT_FUNCTION
extern "C" YARP_IMPORT void yarpcar_INIT_FUNCTION();
#endif
#ifdef yarppm_INIT_FUNCTION
extern "C" YARP_IMPORT void yarppm_INIT_FUNCTION();
#endif
#ifdef yarpmod_INIT_FUNCTION
extern "C" YARP_IMPORT void yarpmod_INIT_FUNCTION();
#endif
#ifdef yarprfmod_INIT_FUNCTION
extern "C" YARP_IMPORT void yarprfmod_INIT_FUNCTION();
#endif

extern "C" void yarpCustomInit() {
#ifdef yarpcar_INIT_FUNCTION
  yarpcar_INIT_FUNCTION();
#endif
#ifdef yarppm_INIT_FUNCTION
  yarppm_INIT_FUNCTION();
#endif
#ifdef yarpmod_INIT_FUNCTION
  yarpmod_INIT_FUNCTION();
#endif
#ifdef yarprfmod_INIT_FUNCTION
  yarprfmod_INIT_FUNCTION();
#endif
}

#include <yarp/os/impl/LogForwarder.h>

extern "C" int __yarp_is_initialized;

extern "C" void yarpCustomFini()
{
}

yarp::os::Network::Network() {
    Network::init();
}

yarp::os::Network::Network(yarp::os::yarpClockType clockType, yarp::os::Clock *custom) {
    init(clockType, custom);
}

yarp::os::Network::~Network() {
    Network::fini();
}

void yarp::os::Network::init() {
    init(yarp::os::YARP_CLOCK_DEFAULT);
}


void yarp::os::Network::init(yarp::os::yarpClockType clockType, yarp::os::Clock *custom) {
    if (__custom_yarp_is_initialized==0) {
        initMinimum(clockType);
        // If we init the clock inside the initMinum, it will loop into itself twice
        // calling again the initMinimun, ending with __yarp_is_initialized counter
        // increased twice.
        yarpCustomInit();
        // NetworkBase::yarpClockInit(clockType, custom);
    }
    __custom_yarp_is_initialized++;
}


void yarp::os::Network::fini() {
    if (__custom_yarp_is_initialized==1) {
        yarpCustomFini();
        finiMinimum();
    }
    if (__custom_yarp_is_initialized>0) __custom_yarp_is_initialized--;
}
