/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#include <yarp/os/LogStream.h>
#include <yarp/os/MemoryGuard.h>
#include <yarp/os/Network.h>
#include <yarp/os/Time.h>

#include <yarp/dev/Drivers.h>

#include "Module.h"


int main(int argc, char* argv[])
{

    // Reserve memory and prefault stack to avoid page faults in the RT control loop
    constexpr std::size_t heap_reserve_bytes = 64 * 1024 * 1024; // 64 MB
    constexpr std::size_t stack_prefault_bytes = 512 * 1024;     // 512 KB
    yarp::os::MemoryGuard memory_guard(stack_prefault_bytes, heap_reserve_bytes);


    yarp::os::Network yarp; // initialize network, this goes before everything


    if (!yarp.checkNetwork()) {
        yFatal() << "Sorry YARP network does not seem to be available, is the yarp server available?";
    }

    yarp::os::ResourceFinder& rf(yarp::os::ResourceFinder::getResourceFinderSingleton());
    rf.setDefaultConfigFile("yarprobotinterface.ini");
    rf.configure(argc, argv);


    // Create and run our module
    yarprobotinterface::Module module;
    return module.runModule(rf);
}
