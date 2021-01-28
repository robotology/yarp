/*
 * Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef YARP_DEV_ICALIBRATOR_H
#define YARP_DEV_ICALIBRATOR_H

#include <yarp/dev/DeviceDriver.h>

namespace yarp {
namespace dev {

class YARP_dev_API ICalibrator
{
public:
    virtual ~ICalibrator() = default;

    virtual bool calibrate(DeviceDriver *dd) = 0;

    virtual bool park(DeviceDriver *dd, bool wait = true) = 0;

    virtual bool quitCalibrate() = 0;

    virtual bool quitPark() = 0;
};

} // namespace dev
} // namespace yarp

#endif // YARP_DEV_ICALIBRATOR_H
