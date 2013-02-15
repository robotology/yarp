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

#define VOCAB_IANALOG VOCAB4('i','a','n','a')
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
    /*
    enum
    {
        AS_OK=0,
        AS_ERROR=1,
        AS_OVF=2,
        AS_TIMEOUT=3
    };
    */
    virtual ~IVirtualAnalogSensor(){}

    /* Set a vector of torque values for virtual sensor
     * @param vals a vector containing the sensor's last readings.
     * @return AS_OK or return code. 
     **/
    //virtual int configure(yarp::os::Searchable &config)=0;
    virtual bool setTorque(yarp::sig::Vector &torques)=0;
    //virtual int getChannels()=0;
};

#endif
//