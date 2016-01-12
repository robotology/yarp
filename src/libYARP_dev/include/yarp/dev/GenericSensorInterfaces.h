// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006 RobotCub Consortium
 * Authors: Lorenzo Natale
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifndef YARPTRACKERINTERFACES
#define YARPTRACKERINTERFACES

#include <yarp/dev/DeviceDriver.h>
#include <yarp/sig/Vector.h>

/*! \file GenericSensorInterfaces.h define interfaces for a generic sensor*/

namespace yarp {
    namespace dev {
        class IGenericSensor;
    }
}

/**
 * @ingroup dev_iface_other
 *
 * A generic interface to sensors -- gyro, a/d converters etc.
 */
class YARP_dev_API yarp::dev::IGenericSensor
{
public:
    virtual ~IGenericSensor(){}

    /**
     * Read a vector from the sensor.
     * @param out a vector containing the sensor's last readings.
     * @return true/false success/failure
     */
    virtual bool read(yarp::sig::Vector &out)=0;

    /**
     * Get the number of channels of the sensor.
     * @param nc pointer to storage, return value
     * @return true/false success/failure
     */
    virtual bool getChannels(int *nc)=0;

    /**
     * Calibrate the sensor, single channel.
     * @param ch channel number
     * @param v reset valure
     * @return true/false success/failure
     */
    virtual bool calibrate(int ch, double v)=0;
};

#endif
