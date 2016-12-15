/*
 * Copyright (C) 2016 iCub Facility - Istituto Italiano di Tecnologia
 * Author: Marco Randazzo <marco.randazzo@iit.it>
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

#ifndef YARP_DEV_ROBOTDESCRIPTIONSERVER_H
#define YARP_DEV_ROBOTDESCRIPTIONSERVER_H


#include <yarp/os/Network.h>
#include <yarp/os/BufferedPort.h>
#include <yarp/dev/PreciselyTimed.h>
#include <yarp/dev/ControlBoardInterfaces.h>
#include <yarp/dev/ControlBoardHelpers.h>
#include <yarp/sig/Vector.h>
#include <yarp/os/Semaphore.h>
#include <yarp/os/Time.h>
#include <yarp/os/ConstString.h>
#include <yarp/dev/PolyDriver.h>
#include <yarp/dev/Wrapper.h>
#include <yarp/dev/IRobotDescription.h>

namespace yarp {
    namespace dev {
        class RobotDescriptionServer;
    }
}

/**
* @ingroup dev_impl_wrapper
*
*/

class yarp::dev::RobotDescriptionServer : public DeviceDriver, public yarp::os::PortReader, public yarp::dev::IMultipleWrapper
                                       
{
#ifndef DOXYGEN_SHOULD_SKIP_THIS
protected:

    yarp::os::Mutex               m_mutex;
    yarp::os::Port                m_rpc_port;
    yarp::os::ConstString         m_local_name;
    yarp::os::ConstString         m_remote_name;
    std::vector<RobotDescription> m_robot_devices;

#endif /*DOXYGEN_SHOULD_SKIP_THIS*/

public:

    /* DeviceDriver methods */
    bool open(yarp::os::Searchable& config);
    bool close();

    virtual bool detachAll();
    virtual bool attachAll(const yarp::dev::PolyDriverList &l);
    virtual bool read(yarp::os::ConnectionReader& connection);

private:
    bool add_device(RobotDescription dev);
    bool remove_device(RobotDescription dev);
};

#endif // YARP_DEV_ROBOTDESCRIPTIONSERVER_H
