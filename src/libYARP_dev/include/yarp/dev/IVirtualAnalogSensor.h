// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2013 RobotCub Consortium
 * Author: Alberto Cardellino
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifndef __YARPVIRTUALANALOGSENSORINTERFACE__
#define __YARPVIRTUALANALOGSENSORINTERFACE__

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

    /* Check the status of a given channel.
    * @param ch: channel number.
    * @return VAS_status type.
    */
    virtual int getState(int ch)=0;

    /* Get the number of channels of the sensor.
     * @return number of channels (0 in case of errors).
     */
    virtual int getChannels()=0;
    
    /* Set a vector of torque values for virtual sensor
     * @param vals a vector containing the sensor's last readings.
     * @return true if ok, false otherwise.
     **/
    virtual bool updateMeasure(yarp::sig::Vector &measure)=0;
    virtual bool updateMeasure(int ch, double &measure)=0;

};

#endif
//
