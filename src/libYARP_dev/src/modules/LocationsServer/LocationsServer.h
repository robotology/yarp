/*
* Copyright (C) 2016 iCub Facility - Istituto Italiano di Tecnologia
* Author: Marco Randazzo <marco.randazzo@iit.it>
* CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
*
*/

#ifndef LOCATIONS_SERVER_H
#define LOCATIONS_SERVER_H

#include <map>
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
#include <yarp/dev/INavigation2D.h>
#include <yarp/os/Publisher.h>
#include <yarp/os/Subscriber.h>
#include <yarp/os/Node.h>
#include "include/visualization_msgs_MarkerArray.h"

namespace yarp {
    namespace dev {
        class LocationsServer;
    }
}

#define DEFAULT_THREAD_PERIOD 20 //ms


/**
* @ingroup dev_impl_wrapper
*
*/

class yarp::dev::LocationsServer : public DeviceDriver , public yarp::os::PortReader

{
#ifndef DOXYGEN_SHOULD_SKIP_THIS
protected:

    yarp::os::Mutex               m_mutex;
    yarp::os::Port                m_rpc_port;
    yarp::os::ConstString         m_local_name;
    int                           m_period;
    yarp::os::Node*               m_rosNode;
    yarp::os::Publisher<visualization_msgs_MarkerArray>   m_rosPublisherPort;

#endif /*DOXYGEN_SHOULD_SKIP_THIS*/

private:
    std::map<std::string, Map2DLocation> m_locations;

public:

    /* DeviceDriver methods */
    bool open(yarp::os::Searchable& config);
    bool close();
    virtual bool read(yarp::os::ConnectionReader& connection);
};

#endif // LOCATIONS_SERVER_H
