// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006 Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the GNU GPL v2.0.
 *
 */

#ifndef __YARP2_DEVICEDRIVER__
#define __YARP2_DEVICEDRIVER__

#include <yarp/os/ConnectionReader.h>
#include <yarp/os/ConnectionWriter.h>
#include <yarp/os/Property.h>

namespace yarp {
    /**
     * An interface for the device drivers 
     */
    namespace dev {
        class DeviceDriver;
    }
}

/**
 * \ingroup dev_class
 *
 * Interface implemented by all device drivers.
 */
class yarp::dev::DeviceDriver
{
public:
    /**
     * Destructor.
     */
    virtual ~DeviceDriver(){}

    /**
     * Open the DeviceDriver. 
     * @param config is a list of parameters for the device.
     * For specic devices, check other open methods for names of parameters
     * and what they mean.  If there are no such open methods,
     * you'll need to read the source code (please nag one of the 
     * yarp developers to add documentation for your device).
     * @return true/false upon success/failure
     */
    virtual bool open(yarp::os::Searchable& config){ return true; }

    /**
     * Close the DeviceDriver.
     * @return true/false on success/failure.
     */
    virtual bool close(){ return true; }

};

#endif
