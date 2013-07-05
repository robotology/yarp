// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright: (C) 2009 RobotCub Consortium
 * Author: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

#include <yarp/os/ConstString.h>
#include <yarp/dev/Drivers.h>
#include <yarp/dev/PolyDriver.h>

namespace yarp {
    namespace dev {
        class DriverCollection;
    }
}

YARP_DECLARE_DEVICES(@libname@);

/**
 *
 * This is an automatically generated class to initialize a collection
 * of drivers.
 *
 * Instantiate it in your main() function as:
 *   yarp::dev::DeviceCollection dev;
 *
 * That's all!  You can print the output of the dev.status() method just to
 * make sure that all the devices you expect to be present actually are.
 *
 * To actually instantiate devices, use the yarp::dev::PolyDriver class.
 *
 */
class yarp::dev::DriverCollection {
public:
    /**
     * Add devices from all imported libraries.
     */
    DriverCollection() {
        YARP_REGISTER_DEVICES(@libname@);
    }

    /**
     * Return a string listing all devices, to allow sanity-checking.
     */
    yarp::os::ConstString status() {
        return yarp::dev::Drivers::factory().toString();
    }
};
