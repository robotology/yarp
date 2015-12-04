// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2015 ICub Facility - Istituto italiano di Tecnologia
 * Author: Marco Randazzo
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifndef __YARPLASERRANGEFINDER2DINTERFACE__
#define __YARPLASERRANGEFINDER2DINTERFACE__

#include <yarp/dev/DeviceDriver.h>
#include <yarp/sig/Vector.h>

#define VOCAB_ILASER2D     VOCAB4('i','l','a','s')
#define VOCAB_DEVICE_INFO VOCAB4('d','v','n','f')

namespace yarp {
    namespace dev {
        class ILaserRangefinder2D;
    }
}

/**
 * @ingroup dev_iface_other
 *
 * A generic interface for planar laser range finders
 */
class YARP_dev_API yarp::dev::ILaserRangefinder2D
{
public:
    enum
    {
        LASER_OK_STANBY        = 0,
        LASER_OK_IN_USE        = 1,
        LASER_GENERAL_ERROR    = 2,
        LASER_TIMEOUT          = 3
    };

    virtual ~ILaserRangefinder2D(){}

    /**
     * Get the instantaneous voltage measurement
     * @param voltage the voltage measurement
     * @return true/false.
     */
    virtual int getRangeData(yarp::sig::Vector &data) = 0;

    /**
    * get the battery status
    * @param status the battery status
    * @return true/false.
    */
    virtual bool getDeviceStatus(int &status) = 0;

    /**
    * get the device hardware charactestics
    * @param a string containing the device infos
    * @return true/false.
    */
    virtual bool getDeviceInfo(yarp::os::ConstString &device_info) = 0;
};

#endif
