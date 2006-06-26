// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-
#ifndef __YARP2_CALLIBRATOR_INTERFACES__
#define __YARP2_CALLIBRATOR_INTERFACES__

#include <yarp/dev/DeviceDriver.h>

namespace yarp {
    namespace dev {
        class ICalibrator;
    }
}

class yarp::dev::ICalibrator
{
 public:
    virtual ~ICalibrator(){}

    virtual bool calibrate(DeviceDriver *dd)=0;
};

#endif
//
