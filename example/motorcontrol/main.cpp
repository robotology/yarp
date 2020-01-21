/*
 * Copyright (C) 2006-2020 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include <yarp/os/Network.h>
#include <yarp/dev/ControlBoardInterfaces.h>
#include <yarp/dev/PolyDriver.h>

using namespace yarp::os;
using namespace yarp::dev;

/** Lorenzo Natale, Dec 2007. Test remotization of calibrate
 *  functions. Useful for general tests on device remotization
 *  (see fakebot for example).
 */
int main(int argc, const char **argv)
{
    Network yarp;
    IControlCalibration2 *ical;

    Property p;
    p.put("device", "remote_controlboard");
    p.put("local", "/motortest");     //prefix for local names
    p.put("remote", "/controlboard"); //prefix for remote names

    PolyDriver device;
    device.open(p);
    device.view(ical);

    //ical->calibrate();
    ical->calibrate2(1,1000,1.1,1.1,1.1);
    ical->done(2);
    ical->park();

    device.close();
    return 0;
}
