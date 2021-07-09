/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_DEV_IVIRTUALANALOGSENSOR_H
#define YARP_DEV_IVIRTUALANALOGSENSOR_H

#include <yarp/os/Vocab.h>
#include <yarp/dev/api.h>
#include <yarp/sig/Vector.h>

constexpr yarp::conf::vocab32_t VOCAB_IVIRTUAL_ANALOG   = yarp::os::createVocab32('i','v','a','n');

/*! \file IVirtualAnalogSensor.h virtual analog sensor interface */
namespace yarp {
    namespace dev {
        class IVirtualAnalogSensor;
        class IVirtualAnalogSensorRaw;
    }
}

namespace yarp {
namespace dev {
YARP_WARNING_PUSH
YARP_DISABLE_CLASS_ENUM_API_WARNING
enum class YARP_dev_API VAS_status
{
    VAS_OK = 0,
    VAS_ERROR = 1,      // generic error
    VAS_OVF = 2,        // overflow
    VAS_TIMEOUT = 3
};
YARP_WARNING_POP
} // namespace dev
} // namespace yarp

/**
 * @ingroup dev_iface_other
 *
 * A generic interface to a virtual sensors. A virtual sensor is any device that
 * can generate values used as a measure by robot
 */

class YARP_dev_API yarp::dev::IVirtualAnalogSensor
{
public:

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

/**
* @ingroup dev_iface_other
*
* A generic interface to a virtual sensors. A virtual sensor is any device that
* can generate values used as a measure by robot
*/

class YARP_dev_API yarp::dev::IVirtualAnalogSensorRaw
{
public:

    virtual ~IVirtualAnalogSensorRaw() {}

    /** Check the status of a given channel.
    * @param ch channel number.
    * @return VAS_status type.
    */
    virtual VAS_status getVirtualAnalogSensorStatusRaw(int ch) = 0;

    /**
    * Get the number of channels of the virtual sensor.
    * @return number of channels (0 in case of errors).
    */
    virtual int getVirtualAnalogSensorChannelsRaw() = 0;

    /**
    * Set a vector of torque values for virtual sensor.
    * @param measure a vector containing the sensor's last readings.
    * @return true if ok, false otherwise.
    */
    virtual bool updateVirtualAnalogSensorMeasureRaw(yarp::sig::Vector &measure) = 0;
    virtual bool updateVirtualAnalogSensorMeasureRaw(int ch, double &measure) = 0;

};

#endif // YARP_DEV_IVIRTUALANALOGSENSOR_H
