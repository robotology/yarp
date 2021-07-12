/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_DEV_IANALOGSENSOR_H
#define YARP_DEV_IANALOGSENSOR_H

#include <yarp/os/Vocab.h>
#include <yarp/dev/api.h>
#include <yarp/sig/Vector.h>

constexpr yarp::conf::vocab32_t VOCAB_IANALOG           = yarp::os::createVocab32('i','a','n','a');

/*! \file IAnalogSensor.h analog sensor interface */

namespace yarp {
namespace dev {

/**
 * @ingroup dev_iface_other
 *
 * A generic interface to sensors (gyro, a/d converters). Similar to
 * IGenerisSensor, but includes error handling.
 */
class YARP_dev_API IAnalogSensor
{
public:
    enum
    {
        AS_OK=0,
        AS_ERROR=1,
        AS_OVF=2,
        AS_TIMEOUT=3
    };

    virtual ~IAnalogSensor();

    /**
     * Read a vector from the sensor.
     * @param out a vector containing the sensor's last readings.
     * @return AS_OK or return code. AS_TIMEOUT if the sensor timed-out.
     */
    virtual int read(yarp::sig::Vector &out)=0;

    /**
     * Check the state value of a given channel.
     * @param ch channel number.
     * @return status.
     */
    virtual int getState(int ch)=0;

    /**
     * Get the number of channels of the sensor.
     * @return number of channels (0 in case of errors).
     */
    virtual int getChannels()=0;

    /**
     * Calibrates the whole sensor.
     * @return status.
     */
    virtual int calibrateSensor()=0;

    /**
     * Calibrates the whole sensor, using an vector of calibration values.
     * @param value a vector of calibration values.
     * @return status.
     */
    virtual int calibrateSensor(const yarp::sig::Vector& value)=0;

    /**
     * Calibrates one single channel.
     * @param ch channel number.
     * @return status.
     */
    virtual int calibrateChannel(int ch)=0;

    /**
     * Calibrates one single channel, using a calibration value.
     * @param ch channel number.
     * @param value calibration value.
     * @return status.
     */
    virtual int calibrateChannel(int ch, double value)=0;
};

} // namespace dev
} // namespace yarp

#endif // YARP_DEV_IANALOGSENSOR_H
