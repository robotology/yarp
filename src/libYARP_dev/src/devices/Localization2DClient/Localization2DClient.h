/*
 * Copyright (C) 2017 iCub Facility - Istituto Italiano di Tecnologia
 * Author: Marco Randazzo <marco.randazzo@iit.it>
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

#ifndef YARP_DEV_LOCALIZATION2DCLIENT_H
#define YARP_DEV_LOCALIZATION2DCLIENT_H


#include <yarp/os/Network.h>
#include <yarp/os/BufferedPort.h>
#include <yarp/dev/PreciselyTimed.h>
#include <yarp/dev/ControlBoardInterfaces.h>
#include <yarp/dev/ControlBoardHelpers.h>
#include <yarp/sig/Vector.h>
#include <yarp/os/Semaphore.h>
#include <yarp/os/Time.h>
#include <yarp/os/Port.h>
#include <yarp/os/Mutex.h>
#include <yarp/os/ConstString.h>
#include <yarp/dev/PolyDriver.h>
#include <yarp/dev/Map2DLocation.h>
#include <yarp/dev/ILocalization2D.h>

namespace yarp {
    namespace dev {
        class Localization2DClient;
    }
}

#define DEFAULT_THREAD_PERIOD 20 //ms


/**
* @ingroup dev_impl_wrapper
*
*/

class yarp::dev::Localization2DClient : public DeviceDriver/*,
                                       public ILocalization2D*/
{
#ifndef DOXYGEN_SHOULD_SKIP_THIS
protected:
    yarp::os::Mutex               m_mutex;
    yarp::os::Port                m_rpc_port_localization_server;
    yarp::os::ConstString         m_local_name;
    yarp::os::ConstString         m_localization_server_name;
    int                           m_period;

#endif /*DOXYGEN_SHOULD_SKIP_THIS*/

public:
    /* DeviceDriver methods */
    bool open(yarp::os::Searchable& config);
    bool close();

    bool   getCurrentPosition(yarp::dev::Map2DLocation &loc);
    bool   setInitialPose(yarp::dev::Map2DLocation& loc);
};

#endif // YARP_DEV_LOCALIZATION2DCLIENT_H
