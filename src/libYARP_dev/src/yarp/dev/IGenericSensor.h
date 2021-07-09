/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_DEV_IGENERICSENSOR_H
#define YARP_DEV_IGENERICSENSOR_H

#include <yarp/dev/api.h>
#include <yarp/sig/Vector.h>

namespace yarp {
namespace dev {

/**
 * @ingroup dev_iface_other
 *
 * A generic interface to sensors -- gyro, a/d converters etc.
 */
class YARP_dev_API IGenericSensor
{
public:
    virtual ~IGenericSensor();

    /**
     * Read a vector from the sensor.
     * @param out a vector containing the sensor's last readings.
     * @return true/false success/failure
     */
    virtual bool read(yarp::sig::Vector &out) = 0;

    /**
     * Get the number of channels of the sensor.
     * @param nc pointer to storage, return value
     * @return true/false success/failure
     */
    virtual bool getChannels(int *nc) = 0;

    /**
     * Calibrate the sensor, single channel.
     * @param ch channel number
     * @param v reset valure
     * @return true/false success/failure
     */
    virtual bool calibrate(int ch, double v) = 0;
};

} // namespace dev
} // namespace yarp

#endif // YARP_DEV_IGENERICSENSOR_H
