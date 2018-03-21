/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef YARP_DEV_IVIRTUALANALOGSENSOR_H
#define YARP_DEV_IVIRTUALANALOGSENSOR_H

#include <yarp/dev/DeviceDriver.h>
#include <yarp/sig/Vector.h>

#define VOCAB_IVIRTUAL_ANALOG VOCAB4('i','v','a','n')
#define VOCAB_CALIBRATE_CHANNEL VOCAB4('c','a','l','c')

/*! \file IVirtualAnalogSensor.h virtual analog sensor interface */
namespace yarp {
    namespace dev {
        class IVirtualAnalogSensor;
    }
}

/**
 * @ingroup dev_iface_other
 *
 * A generic interface to a virtual sensors. A virtual sensor is any device that
 * can generate values used as a measure by robot
 */

class YARP_dev_API yarp::dev::IVirtualAnalogSensor
{
public:
    enum VAS_status
    {
        VAS_OK=0,
        VAS_ERROR=1,      // generic error
        VAS_OVF=2,        // overflow
        VAS_TIMEOUT=3
    };

    virtual ~IVirtualAnalogSensor(){}

    /** Check the status of a given channel.
     * @param ch channel number.
     * @return VAS_status type.
     */
    virtual VAS_status getVirtualAnalogSensorStatus(int ch)=0;

    /**
     * Get the number of channels of the virtual sensor.
     * @return number of channels (0 in case of errors).
     */
    virtual int getVirtualAnalogSensorChannels()=0;

    /**
     * Set a vector of torque values for virtual sensor.
     * @param measure a vector containing the sensor's last readings.
     * @return true if ok, false otherwise.
     */
    virtual bool updateVirtualAnalogSensorMeasure(yarp::sig::Vector &measure)=0;
    virtual bool updateVirtualAnalogSensorMeasure(int ch, double &measure)=0;

};

#endif // YARP_DEV_IVIRTUALANALOGSENSOR_H
