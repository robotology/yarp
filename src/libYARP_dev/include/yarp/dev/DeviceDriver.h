// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-
#ifndef __YARP2_DEVICEDRIVER__
#define __YARP2_DEVICEDRIVER__

#include <yarp/os/ConnectionReader.h>
#include <yarp/os/ConnectionWriter.h>
#include <yarp/os/Property.h>

namespace yarp {
    /**
     * An interface for the device drivers 
     */
    namespace dev {
        class DeviceDriver;
    }
}

/**
 * \ingroup dev_class
 *
 * Interface implemented by all device drivers.
 */
class yarp::dev::DeviceDriver
{
public:
    /**
     * Destructor.
     */
    virtual ~DeviceDriver(){}

    /**
     * Open the DeviceDriver. 
     * @param config is a list of parameters for the device.
     * @return true/false upon success/failure
     */
    virtual bool open(yarp::os::Searchable& config){ return true; }

    /**
     * Close the DeviceDriver.
     * @return true/false on success/failure.
     */
    virtual bool close(){ return true; }

    /**
     * Generic command interface.
     * @return true on success.
     */
    //virtual bool read(yarp::os::ConnectionReader& cmd);

    /**
     * Generic command interface.
     * @return true on success.
     */
    //deprecated
    //virtual bool apply(yarp::os::Bottle& cmd, 
    //                 yarp::os::Bottle& response);

};

#endif
