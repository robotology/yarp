/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_DEV_ICALIBRATOR_H
#define YARP_DEV_ICALIBRATOR_H

#include <yarp/dev/DeviceDriver.h>
#include <yarp/dev/ReturnValue.h>

namespace yarp::dev {

/**
 * @ingroup dev_iface_motor
 *
 * Interface for a calibrator device.
 */

class YARP_dev_API ICalibrator
{
public:
    virtual ~ICalibrator() = default;

    virtual yarp::dev::ReturnValue calibrate(DeviceDriver *dd) = 0;

    virtual yarp::dev::ReturnValue park(DeviceDriver *dd, bool wait = true) = 0;

    virtual yarp::dev::ReturnValue quitCalibrate() = 0;

    virtual yarp::dev::ReturnValue quitPark() = 0;
};

} // namespace yarp::dev

#endif // YARP_DEV_ICALIBRATOR_H
