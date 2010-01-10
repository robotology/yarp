// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2010 RobotCub Consortium
 * Author: Lorenzo Natale
 * CopyPolicy: Released under the terms of the GNU GPL v2.0.
 *
 */

#ifndef __YARPANALOGSENSORINTERFACE__
#define __YARPANALOGSENSORINTERFACE__

#include <yarp/dev/DeviceDriver.h>
#include <yarp/sig/Vector.h>

/*! \file IAnalogSensor.h analog sensor interface */
namespace yarp {
    namespace dev {
        class IAnalogSensor;
        enum
        {
            AS_OK=1,
            AS_OFV=2,
            AS_TIMEOUT=3
        }
    }
}

/**
 * @ingroup dev_iface_other
 *
 * A generic interface to sensors (gyro, a/d converters). Similar to
 * IGenerisSensor, but includes error handling.
 */
class yarp::dev::IAnalogSensor
{
public:
    virtual ~IAnalogSensor(){}

    /* Read a vector from the sensor.
     * @param out a vector containing the sensor's last readings.
     * @return AS_OK or return code. AS_OVF if overflow ocurred, AS_TIMEOUT if the sensor timed-out.
     **/
    virtual int read(yarp::sig::Vector &out)=0;
   
    /* Check the state value of a given channel.
    * @param ch: channel number.
    * @return status.
    */
    virtual int getState(int ch)=0;
    
    /* Get the number of channels of the sensor.
     * @return number of channels (0 in case of errors).
     */
    virtual int getChannels()=0;
};

#endif
//
