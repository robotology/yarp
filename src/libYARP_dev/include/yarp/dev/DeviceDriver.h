// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-
#ifndef __YARP2_DEVICEDRIVER__
#define __YARP2_DEVICEDRIVER__

namespace yarp {
    /**
     * An interface for the device drivers 
     */
    namespace dev {
        class DeviceDriver;
    }
}

/**
 * Interface implemented by all device drivers.
 * At the moment this is an empty class, in the future it might contain
 * methods common to all device drivers (e.g. serialization/deserialization)
 */
class yarp::dev::DeviceDriver 
{
public:
    virtual ~DeviceDriver(){}
    /**
     * Open the DeviceDriver. Specific devices might define 
     * more suitable methods, with parameters.
     * @return true/false upon success/failure
     */
    virtual bool open(){return true;}

    /**
     * Close the DeviceDriver.
     * @return true/false on success/failure.
     */
    virtual bool close()=0;
};

#endif
