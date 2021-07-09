/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/companion/impl/Companion.h>

#include <yarp/conf/environment.h>

#include <yarp/os/Bottle.h>
#include <yarp/os/BufferedPort.h>
#include <yarp/os/LogStream.h>
#include <yarp/os/Network.h>
#include <yarp/os/Os.h>
#include <yarp/os/Property.h>
#include <yarp/os/SystemClock.h>
#include <yarp/os/Value.h>

#include <cmath>

using yarp::companion::impl::Companion;
using yarp::os::Bottle;
using yarp::os::BufferedPort;
using yarp::os::NetworkBase;
using yarp::os::Property;
using yarp::os::SystemClock;
using yarp::os::Value;

int Companion::cmdClock(int argc, char *argv[])
{
    double init, offset;
    Property config;
    SystemClock clock;
    std::string portName;
    yarp::os::BufferedPort<yarp::os::Bottle> streamPort;

    config.fromCommand(argc, argv, false, true);
    double period = config.check("period", Value(30), "update period, default 30ms").asFloat64() /1000.0;
    double timeFactor = config.check("rtf", Value(1), "real time factor. Upscale or downscale the clock frequency by a multiplier factor. Default 1").asFloat64();
    bool system = config.check("systemTime", "Publish system clock. If false time starts from zero. Default false");
    bool help = config.check("help");

    if(help)
    {
        yCInfo(COMPANION, "This command publishes a clock time through a YARP port");
        yCInfo(COMPANION);
        yCInfo(COMPANION, "Accepted parameters are:");
        yCInfo(COMPANION, "period:     update period [ms]. Default 30");
        yCInfo(COMPANION, "name:       name of yarp port to be opened. Default: check YARP_CLOCK environment variable; if missing use '/clock'");
        yCInfo(COMPANION, "rtf:        realt time factor. Elapsed time will be multiplied by this factor to simulate faster or slower then real time clock frequency. Default 1 (real time)");
        yCInfo(COMPANION, "systemTime: If present the published time will start at the same value as system clock. If if not present (default) the published time will start from 0. ");
        yCInfo(COMPANION, "help:       print this help");
        yCInfo(COMPANION);
        return 1;
    }

    /* Determine clock port name.
     *
     * If the user specify a name, use it.
     * If not, we check the environment variable.
     * If no env variable is present, use the '/clock' as fallback.
     */
    portName = yarp::conf::environment::get_string("YARP_CLOCK");
    if (portName.empty()) {
        portName = "/clock";
    }

    portName = config.check("name", Value(portName), "name of port broadcasting the time").asString();

    yCInfo(COMPANION, "Clock configuration is the following:");
    yCInfo(COMPANION, "period %.3f msec", period*1000);
    yCInfo(COMPANION, "name   %s", portName.c_str());
    yCInfo(COMPANION, "rtf    %.3f", timeFactor);
    yCInfo(COMPANION, "system %s", system ? "true" : "false");

    if(!streamPort.open(portName) )
    {
        yCInfo(COMPANION, "yarp clock error: Cannot open '/clock' port");
        return 1;
    }

    yCInfo(COMPANION);
    yCInfo(COMPANION);
    double sec, nsec, elapsed;
    double time = clock.now();

    if(system)
    {
        init = time;
        offset = time;
    }
    else
    {
        init = time;
        offset = 0;
    }

    bool done = false;
    while (true)
    {
        elapsed = clock.now() - init;
        time = elapsed * timeFactor + offset;
        Bottle &tick = streamPort.prepare();
        // convert time to sec, nsec
        nsec = std::modf(time, &sec) *1e9;

        tick.clear();
        tick.addInt32((int32_t)sec);
        tick.addInt32((int32_t)nsec);
        streamPort.write();

        if( (((int) elapsed %5) == 0))
        {
            if(!done) {
                yCInfo(COMPANION, "yarp clock running happily...");
                std::fflush(stdout);
            }
            done = true;
        }
        else
            done = false;

        clock.delay(period);
    }
    return 0;
}
