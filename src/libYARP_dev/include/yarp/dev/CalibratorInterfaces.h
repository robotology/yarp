// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006 RobotCub Consortium
 * Authors: Lorenzo Natale
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifndef __YARP2_CALLIBRATOR_INTERFACES__
#define __YARP2_CALLIBRATOR_INTERFACES__

#include <yarp/dev/DeviceDriver.h>

namespace yarp {
    namespace dev {
        class ICalibrator;
    }
}

class YARP_dev_API yarp::dev::ICalibrator
{
public:
    virtual ~ICalibrator(){}

    virtual bool calibrate(DeviceDriver *dd)=0;

    virtual bool park(DeviceDriver *dd, bool wait=true)=0;

    virtual bool quitCalibrate()=0;

    virtual bool quitPark()=0;
};

#endif
