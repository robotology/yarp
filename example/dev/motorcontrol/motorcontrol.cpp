/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/os/Network.h>
#include <yarp/os/Property.h>

#include <yarp/dev/IControlCalibration.h>
#include <yarp/dev/PolyDriver.h>

int main(int argc, const char* argv[])
{
    YARP_UNUSED(argc);
    YARP_UNUSED(argv);

    yarp::os::Network yarp;
    yarp::dev::IControlCalibration* ical;

    yarp::os::Property p;
    p.put("device", "remote_controlboard");
    p.put("local", "/motortest");     //prefix for local names
    p.put("remote", "/controlboard"); //prefix for remote names

    yarp::dev::PolyDriver device;
    device.open(p);
    device.view(ical);

    ical->calibrateAxisWithParams(1, 1000, 1.1, 1.1, 1.1);
    ical->calibrationDone(2);
    ical->park();

    device.close();
    return 0;
}
