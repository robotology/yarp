// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-
#ifndef __YARPTRACKERINTERFACES__
#define __YARPTRACKERINTERFACES__

#include <yarp/dev/DeviceDriver.h>
#include <yarp/sig/Vector.h>

/*! \file GenericSensorInterfaces.h define interfaces for a generic sensor*/

namespace yarp {
    namespace dev {
        class IGenericSensor;
    }
}

/* IGenericSensor, a generic interface to sensors (gyro, ad converters...) */
class yarp::dev::IGenericSensor
{
public:
    ~IGenericSensor(){}

    /* Read a vector from the sensor.
     * @param out a vector containing the sensor's last readings.
     * @return true/false success/failure
     **/
    virtual bool read(yarp::sig::Vector &out)=0;
    
    /* Get the number of channels of the sensor.
     * @param nc pointer to storage, return value
     * @return true/false success/failure
     */
    virtual bool getChannels(int *nc)=0;
};

#endif
//
